/*
 * svd_model_computation.cpp
 * Created on: Jan 8, 2015
 * Author: chenguangyu
 */

#include "svd_model_computation.h"
#include "common/lang/integer.h"
#include "common/math/math.h"
#include "common/util/random.h"
#include "common/logging/logging.h"

namespace longan {

namespace SVD {

void FPSGDModelComputation::ComputeModel(const TrainOption* trainOption, const GriddedMatrix* griddedMatrix, ModelTrain* model) {
    Log::I("recsys", "FPSGDModelComputation::ComputeModel()");
    mTrainOption = trainOption;
    mGriddedMatrix = griddedMatrix;
    mModel = model;
    Init();
    CreateScheduler();
    mScheduler->Start();
    mScheduler->WaitFinish();
    Cleanup();
}

void FPSGDModelComputation::Init() {
    mGridRowBlocked.assign(mTrainOption->NumUserBlock(), false);
    mGridColBlocked.assign(mTrainOption->NumItemBlock(), false);
    mGridIterationCount.assign(mTrainOption->NumUserBlock() * mTrainOption->NumItemBlock(), 0);
}

void FPSGDModelComputation::CreateScheduler() {
    int numProducer = 1;
    int numWorker = mTrainOption->NumThread();
    int numConsumer = 1;
    int bufferSize1 = 1;
    int bufferSize2 = 1;
    mScheduler = new PipelinedScheduler<Task>(this, numProducer, numWorker, numConsumer,
            bufferSize1, bufferSize2);
}

void FPSGDModelComputation::Cleanup() {
    delete mScheduler;
}

void FPSGDModelComputation::ProducerRun() {
    int rows = mTrainOption->NumUserBlock();
    int cols = mTrainOption->NumItemBlock();
    int iterationLimit = mTrainOption->Iterations();
    std::vector<int> candidates;
    candidates.reserve(rows * cols);
    while (true) {
        int minCount = Integer::MAX;
        candidates.clear();
        for (int i = 0; i < rows; ++i) {
            if (mGridRowBlocked[i]) continue;
            for (int j = 0; j < cols; ++j) {
                if(mGridColBlocked[j]) continue;
                int gridId = i * cols + j;
                int iterationCount = mGridIterationCount[gridId];
                if (iterationCount >= iterationLimit) continue;
                if (iterationCount == minCount) {
                    candidates.push_back(gridId);
                } else if(iterationCount < minCount) {
                    minCount = iterationCount;
                    candidates.clear();
                    candidates.push_back(gridId);
                }
            }
        }
        if (candidates.size() > 0) {
            int chosenGridId = candidates[Random::Instance().Uniform(0, candidates.size())];
            mGridRowBlocked[chosenGridId / cols] = true;
            mGridColBlocked[chosenGridId % cols] = true;
            ++mGridIterationCount[chosenGridId];
            Task* task = new Task();
            task->gridId = chosenGridId;
            mScheduler->ProducerPutTask(task);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        mCurrentIteration = Math::MinInRange(mGridIterationCount.begin(), mGridIterationCount.end());
        if (mCurrentIteration >= iterationLimit) break;
    }
    mScheduler->ProducerDone();
}

void FPSGDModelComputation::WorkerRun() {
    int dim = mModel->mParameter.Dim();
    bool enableUserBias = (mModel->mParameter.LambdaUserBias() >= 0.0f);
    bool enableItemBias = (mModel->mParameter.LambdaItemBias() >= 0.0f);
    float ratingAverage = mTrainOption->UseRatingAverage() ? mModel->mRatingAverage : 0.0f;
    float learningRate = mTrainOption->LearningRate();
    float glp = 1 - learningRate * (mModel->mParameter.LambdaUserFeature());
    float glq = 1 - learningRate * (mModel->mParameter.LambdaItemFeature());
    float glub = 1 - learningRate * (mModel->mParameter.LambdaUserBias());
    float glib = 1 - learningRate * (mModel->mParameter.LambdaItemBias());
    float *ub, *ib;
    while(true) {
        Task *task = mScheduler->WorkerGetTask();
        if (task == nullptr) break;
        int gridId = task->gridId;
        const Matrix& mat = mGriddedMatrix->Grid(gridId);
        for(int i = 0; i < mat.NumRating(); ++i) {
            const Node& node = mat.Get(i);
            Vector<float>& userFeature = mModel->mUserFeatures[node.UserId()];
            Vector<float>& itemFeature = mModel->mItemFeatures[node.ItemId()];
            float ge = InnerProd(userFeature, itemFeature) + ratingAverage;
            if (enableUserBias) {
                ub = &(mModel->mUserBiases[node.UserId()]);
                ge += *ub;
            }
            if (enableItemBias) {
                ib = &(mModel->mItemBiases[node.ItemId()]);
                ge += *ib;
            }
            ge = node.Rating() - ge;
            ge *= learningRate;
            for (int d = 0; d < dim; d++) {
                float tmp = userFeature[d];
                userFeature[d] = ge*itemFeature[d] + glp * userFeature[d];
                itemFeature[d] = ge*tmp + glq*itemFeature[d];
            }
            if (enableUserBias) {
                *ub = glub * (*ub) + ge;
            }
            if (enableItemBias) {
                *ib = glib * (*ib) + ge;
            }
        }
        mScheduler->WorkerPutTask(task);
    }
    mScheduler->WorkerDone();
}

void FPSGDModelComputation::ConsumerRun() {
    int rows = mTrainOption->NumUserBlock();
    int cols = mTrainOption->NumItemBlock();
    while (true) {
        Task *task = mScheduler->ConsumerGetTask();
        if (task == nullptr) break;
        int gridId = task->gridId;
        mGridRowBlocked[gridId / cols] = false;
        mGridColBlocked[gridId % cols] = false;
        delete task;
    }
    mScheduler->ConsumerDone();
}

void FPSGDModelComputation::MonitorRun() {
    while (true) {
        ConsoleLog::I("recsys", "Computing Model...iteration at "
                       + Integer::ToString(mCurrentIteration) + "/"
                       + Integer::ToString(mTrainOption->Iterations()));
        if (mCurrentIteration >= mTrainOption->Iterations() - 1) break;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

} //~ namespace SVD

//void Monitor::print(int const iter, float const time, double const loss,
//                    float const tr_rmse)
//{
//    char output[1024];
//    sprintf(output, "%-4d %10.2f", iter, time);
//    if(show_tr_rmse)
//        sprintf(output+strlen(output), " %10.3f", tr_rmse);
//    if(Va != nullptr)
//        sprintf(output+strlen(output), " %10.3f", calc_rmse(*model, *Va));
//    if(show_obj)
//    {
//        double const reg = calc_reg();
//        sprintf(output+strlen(output), " %13.3e %13.3e %13.3e", loss, reg,
//                loss+reg);
//    }
//    printf("%s\n", output);
//    fflush(stdout);
//}
//
//double Monitor::calc_reg()
//{
//    int const dim_aligned = get_aligned_dim(model->param.dim);
//    double reg = 0;
//
//    {
//        float * const P = model->P;
//        double reg_p = 0;
//        for(int u = 0; u < model->nr_users; u++)
//        {
//            float * const p = P+u*dim_aligned;
//            reg_p += nr_ratings_per_user[u] *
//                     std::inner_product(p, p+model->param.dim, p, 0.0);
//        }
//        reg += reg_p*model->param.lp;
//    }
//
//    {
//        float * const Q = model->Q;
//        double reg_q = 0;
//        for(int i = 0; i < model->nr_items; i++)
//        {
//            float * const q = Q+i*dim_aligned;
//            reg_q += nr_ratings_per_item[i] *
//                     std::inner_product(q, q+model->param.dim, q, 0.0);
//        }
//        reg += reg_q*model->param.lq;
//    }
//
//    if(model->param.lub >= 0)
//    {
//        double reg_ub = 0;
//        for(int u = 0; u < model->nr_users; u++)
//            reg_ub += nr_ratings_per_user[u] * model->UB[u] * model->UB[u];
//        reg += reg_ub*model->param.lub;
//    }
//
//    if(model->param.lib >= 0)
//    {
//        double reg_ib = 0;
//        for(int i = 0; i < model->nr_items; i++)
//            reg_ib += nr_ratings_per_item[i] * model->UB[i] * model->UB[i];
//        reg += reg_ib*model->param.lib;
//    }
//
//    return reg;
//}
//float  Monitor::calc_rate(Model const &model, Node const &r)
//{
//    int const dim_aligned = get_aligned_dim(model.param.dim);
//    float rate = std::inner_product(
//                     model.P+r.uid*dim_aligned,
//                     model.P+r.uid*dim_aligned + model.param.dim,
//                     model.Q+r.iid*dim_aligned,
//                     0.0);
//    rate += model.avg;
//    if(model.param.lub >= 0)
//        rate += model.UB[r.uid];
//    if(model.param.lib >= 0)
//        rate += model.IB[r.iid];
//    return rate;
//}
//
//float  Monitor::calc_rmse(const Model& model, Matrix const &M)
//{
//    double loss = 0;
//    for(auto r = M.R.begin(); r != M.R.end(); r++)
//    {
//        float const e = r->rate - calc_rate(model, *r);
//        loss += e*e;
//    }
//    return sqrt(loss/M.nr_ratings);
//}

} //~ namespace longan

