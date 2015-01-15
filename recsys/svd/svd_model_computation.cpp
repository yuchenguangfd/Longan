/*
 * svd_model_computation.cpp
 * Created on: Jan 8, 2015
 * Author: chenguangyu
 */

#include "svd_model_computation.h"
#include "common/lang/integer.h"
#include "common/math/math.h"
#include "common/util/random.h"
#include "common/util/array_helper.h"
#include "common/logging/logging.h"

namespace longan {

namespace SVD {

void SimpleModelComputation::ComputeModel(const TrainOption *trainOption, const GriddedMatrix *griddedMatrix,
        ModelTrain *model) {
    Log::I("recsys", "SimpleModelComputation::ComputeModel()");
    int dim = model->mParameter.Dim();
    bool enableUserBias = (model->mParameter.LambdaUserBias() >= 0.0f);
    bool enableItemBias = (model->mParameter.LambdaItemBias() >= 0.0f);
    float ratingAverage = trainOption->UseRatingAverage() ? model->mRatingAverage : 0.0f;
    float learningRate = trainOption->LearningRate();
    float glp = 1 - learningRate * (model->mParameter.LambdaUserFeature());
    float glq = 1 - learningRate * (model->mParameter.LambdaItemFeature());
    float glub = 1 - learningRate * (model->mParameter.LambdaUserBias());
    float glib = 1 - learningRate * (model->mParameter.LambdaItemBias());
    float *ub, *ib;
    std::vector<int> gridIdOrder(trainOption->NumUserBlock() * trainOption->NumItemBlock());
    ArrayHelper::FillRange(gridIdOrder.data(), gridIdOrder.size());
    for (int iter = 0; iter < trainOption->Iterations(); ++iter) {
        Log::I("recsys", "iteration " + Integer::ToString(iter));
        ArrayHelper::RandomShuffle(gridIdOrder.data(), gridIdOrder.size());
        for (int ii = 0; ii < gridIdOrder.size(); ++ii) {
            const Matrix& mat = griddedMatrix->Grid(gridIdOrder[ii]);
            for(int i = 0; i < mat.NumRating(); ++i) {
                const Node& node = mat.Get(i);
                Vector<float>& userFeature = model->mUserFeatures[node.UserId()];
                Vector<float>& itemFeature = model->mItemFeatures[node.ItemId()];
                float ge = InnerProd(userFeature, itemFeature) + ratingAverage;
                if (enableUserBias) {
                    ub = &(model->mUserBiases[node.UserId()]);
                    ge += *ub;
                }
                if (enableItemBias) {
                    ib = &(model->mItemBiases[node.ItemId()]);
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
        }
    }
}

void FPSGDModelComputation::ComputeModel(const TrainOption *trainOption, const GriddedMatrix *griddedMatrix,
        ModelTrain *model) {
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
            {
                std::unique_lock<std::mutex> lock(mGridBlockedMutex);
                mGridRowBlocked[chosenGridId / cols] = true;
                mGridColBlocked[chosenGridId % cols] = true;
            }
            ++mGridIterationCount[chosenGridId];
            Task* task = new Task();
            task->gridId = chosenGridId;
            mScheduler->ProducerPutTask(task);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
        {
            std::unique_lock<std::mutex> lock(mGridBlockedMutex);
            mGridRowBlocked[gridId / cols] = false;
            mGridColBlocked[gridId % cols] = false;
        }
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

} //~ namespace longan

