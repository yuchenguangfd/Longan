/*
 * svd_model_computation.cpp
 * Created on: Jan 8, 2015
 * Author: chenguangyu
 */

#include "svd_model_computation.h"
#include "common/common.h"

namespace longan {

namespace SVD {

/*
 * Optimize Function:
 * for numercial:
 * E = sum{
 *         (Rating[uv] - inner_prod(P[u],Q[v]) - UserBias[u] - ItemBias[v] - RatingAverage)^2
 *       + lp*norm(P[u]) + lq*norm(Q[v]) + lub*norm(UserBias) + lib*norm(ItemBias)
 * }
 * for binary:
 * E = sum{
 *         (Rating[uv] - sigmoid(inner_prod(P[u],Q[v])+UserBias[u]+ItemBias[v]))^2
 *       + lp*norm(P[u]) + lq*norm(Q[v]) + lub*norm(UserBias) + lib*norm(ItemBias)
 * }
 */
void ModelComputation::SGDOneMatrix(int gridId) {
    const Matrix& mat = mTrainData->Get(gridId);
    float ratingAverage = mModel->mParameter->UseRatingAverage() ? mModel->mRatingAverage : 0.0f;
    bool enableUserBias = (mModel->mParameter->LambdaUserBias() >= 0.0f);
    bool enableItemBias = (mModel->mParameter->LambdaItemBias() >= 0.0f);
    bool useSigmoid = mModel->mParameter->UseSigmoid();
    float *pUserBias, *pItemBias;
    float lr = mTrainOption->LearningRate();
    float ge;
    float glp = 1 - lr * (mModel->mParameter->LambdaUserFeature());
    float glq = 1 - lr * (mModel->mParameter->LambdaItemFeature());
    float glub = 1 - lr * (mModel->mParameter->LambdaUserBias());
    float glib = 1 - lr * (mModel->mParameter->LambdaItemBias());
    int dim = mModel->mParameter->Dim();
    for (int i = 0; i < mat.NumRating(); ++i) {
        const Node& node = mat.Get(i);
        Vector32F& userFeature = mModel->mUserFeatures[node.UserId()];
        Vector32F& itemFeature = mModel->mItemFeatures[node.ItemId()];
        float predRating = InnerProd(userFeature, itemFeature) + ratingAverage;
        if (enableUserBias) {
            pUserBias = &(mModel->mUserBiases[node.UserId()]);
            predRating += *pUserBias;
        }
        if (enableItemBias) {
            pItemBias = &(mModel->mItemBiases[node.ItemId()]);
            predRating += *pItemBias;
        }
        if (useSigmoid) {
            predRating = Math::Sigmoid(predRating);
            ge = (node.Rating() - predRating) * predRating * (1 - predRating) * lr;
        } else {
            ge = (node.Rating() - predRating) * lr;
        }
        for (int d = 0; d < dim; d++) {
            float tmp = userFeature[d];
            userFeature[d] = ge * itemFeature[d] + glp * userFeature[d];
            itemFeature[d] = ge * tmp + glq * itemFeature[d];
        }
        if (enableUserBias) {
            *pUserBias = glub * (*pUserBias) + ge;
        }
        if (enableItemBias) {
            *pItemBias = glib * (*pItemBias) + ge;
        }
    }
}

void ModelComputation::ComputeLossInit() {
    mNumRatingPerUser.assign(mTrainData->NumUser(), 0);
    mNumRatingPerItem.assign(mTrainData->NumItem(), 0);
    for (int gridId = 0; gridId < mTrainData->NumBlock(); ++gridId) {
        const Matrix& mat = mTrainData->Get(gridId);
        for (int i = 0; i < mat.NumRating(); ++i) {
            const Node& node = mat.Get(i);
            ++mNumRatingPerUser[node.UserId()];
            ++mNumRatingPerItem[node.ItemId()];
        }
    }
}

void ModelComputation::ComputeLoss(double *dataLoss, double *regLoss, double *validateLoss) {
    *dataLoss = ComputeDataLoss();
    *regLoss = ComputeRegLoss();
    *validateLoss = ComputeValidateLoss();
}

double ModelComputation::ComputeDataLoss() {
    float ratingAverage = mModel->mParameter->UseRatingAverage() ? mModel->mRatingAverage : 0.0f;
    bool enableUserBias = (mModel->mParameter->LambdaUserBias() >= 0.0f);
    bool enableItemBias = (mModel->mParameter->LambdaItemBias() >= 0.0f);
    bool useSigmoid = mModel->mParameter->UseSigmoid();
    double loss = 0.0;
    for (int gridId = 0; gridId < mTrainData->NumBlock(); ++gridId) {
        const Matrix& mat = mTrainData->Get(gridId);
        for (int i = 0; i < mat.NumRating(); ++i) {
            const Node& node = mat.Get(i);
            Vector32F& userFeature = mModel->mUserFeatures[node.UserId()];
            Vector32F& itemFeature = mModel->mItemFeatures[node.ItemId()];
            float predRating = InnerProd(userFeature, itemFeature) + ratingAverage;
            if (enableUserBias) {
                predRating += mModel->mUserBiases[node.UserId()];
            }
            if (enableItemBias) {
                predRating += mModel->mItemBiases[node.ItemId()];
            }
            if (useSigmoid) {
                predRating = Math::Sigmoid(predRating);
            }
            loss += Math::Sqr(node.Rating() - predRating);
        }
    }
    return loss;
}

double ModelComputation::ComputeRegLoss() {
    bool enableUserBias = (mModel->mParameter->LambdaUserBias() >= 0.0f);
    bool enableItemBias = (mModel->mParameter->LambdaItemBias() >= 0.0f);
    double loss = 0.0;
    float lp = mModel->mParameter->LambdaUserFeature();
    float lq = mModel->mParameter->LambdaItemFeature();
    float lub = mModel->mParameter->LambdaUserBias();
    float lib = mModel->mParameter->LambdaItemBias();
    for (int uid = 0; uid < mModel->mNumUser; ++uid) {
        const Vector32F& userFeature = mModel->mUserFeatures[uid];
        loss += mNumRatingPerUser[uid] * lp * NormL2Sqr(userFeature);
    }
    for (int iid = 0; iid < mModel->mNumItem; ++iid) {
        const Vector32F& itemFeature = mModel->mItemFeatures[iid];
        loss += mNumRatingPerItem[iid] * lq * NormL2Sqr(itemFeature);
    }
    if (enableUserBias) {
        double regub = 0.0;
        for (int uid = 0; uid < mModel->mNumUser; ++uid) {
            regub += mNumRatingPerUser[uid] * Math::Sqr(mModel->mUserBiases[uid]);
        }
        loss += lub * regub;
    }
    if (enableItemBias) {
        double regib = 0.0;
        for (int iid = 0; iid < mModel->mNumItem; ++iid) {
            regib += mNumRatingPerItem[iid] * Math::Sqr(mModel->mItemBiases[iid]);
        }
        loss += lib * regib;
    }
    return loss;
}

double ModelComputation::ComputeValidateLoss() {
    float ratingAverage = mModel->mParameter->UseRatingAverage() ? mModel->mRatingAverage : 0.0f;
    bool enableUserBias = (mModel->mParameter->LambdaUserBias() >= 0.0f);
    bool enableItemBias = (mModel->mParameter->LambdaItemBias() >= 0.0f);
    bool useSigmoid = mModel->mParameter->UseSigmoid();
    double loss = 0.0;
    for (int gridId = 0; gridId < mValidateData->NumBlock(); ++gridId) {
        const Matrix& mat = mValidateData->Get(gridId);
        for (int i = 0; i < mat.NumRating(); ++i) {
            const Node& node = mat.Get(i);
            Vector32F& userFeature = mModel->mUserFeatures[node.UserId()];
            Vector32F& itemFeature = mModel->mItemFeatures[node.ItemId()];
            float predRating = InnerProd(userFeature, itemFeature) + ratingAverage;
            if (enableUserBias) {
                predRating += mModel->mUserBiases[node.UserId()];
            }
            if (enableItemBias) {
                predRating += mModel->mItemBiases[node.ItemId()];
            }
            if (useSigmoid) {
                predRating = Math::Sigmoid(predRating);
            }
            loss += Math::Sqr(node.Rating() - predRating);
        }
    }
    return loss;
}

void ModelComputationST::ComputeModel(const TrainOption *option, const GriddedMatrix *trainData,
        const GriddedMatrix *validateData, ModelTrain *model) {
    Log::I("recsys", "ModelComputationST::ComputeModel()");
    mTrainOption = option;
    mTrainData = trainData;
    mValidateData = validateData;
    mModel = model;
    if (mTrainOption->MonitorIteration()) {
        ComputeLossInit();
    }
    std::vector<int> order(mTrainData->NumBlock());
    ArrayHelper::FillRange(order.data(), order.size());
    for (int iter = 0; iter < mTrainOption->Iterations(); ++iter) {
        ArrayHelper::RandomShuffle(order.data(), order.size());
        for (int i = 0; i < order.size(); ++i) {
            SGDOneMatrix(order[i]);
        }
        if (mTrainOption->MonitorIteration()
            && iter % mTrainOption->MonitorIterationStep() == 0) {
            double dataLoss, regLoss, validateLoss;
            ComputeLoss(&dataLoss, &regLoss, &validateLoss);
            Log::I("recsys", "iteration %d/%d done. \n"
                   "data loss = %lf\t reg loss = %lf\t train loss = %lf\t validate loss = %lf",
                   iter, mTrainOption->Iterations(),
                   dataLoss, regLoss, dataLoss + regLoss, validateLoss);
        }
    }
}

void ModelComputationMT::ComputeModel(const TrainOption *option, const GriddedMatrix *trainData,
        const GriddedMatrix *validateData, ModelTrain *model) {
    Log::I("recsys", "ModelComputationMT::ComputeModel()");
    mTrainOption = option;
    mTrainData = trainData;
    mValidateData = validateData;
    mModel = model;
    if (mTrainOption->MonitorIteration()) {
        ComputeLossInit();
    }
    mGridRowBlocked.assign(mTrainOption->NumUserBlock(), false);
    mGridColBlocked.assign(mTrainOption->NumItemBlock(), false);
    mGridIterationCount.assign(mTrainOption->NumUserBlock() * mTrainOption->NumItemBlock(), 0);
    mScheduler = new PipelinedScheduler<Task>(this, 1, mTrainOption->NumThread(), 1, 1, 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
}

void ModelComputationMT::ProducerRun() {
    std::vector<int> candidates;
    candidates.reserve(mTrainData->NumBlock());
    int currIter = 0, lastIter = -1;
    while (true) {
        int minIterationCount = Integer::MAX;
        candidates.clear();
        for (int i = 0; i < mTrainData->NumUserBlock(); ++i) {
            if (mGridRowBlocked[i]) continue;
            for (int j = 0; j < mTrainData->NumItemBlock(); ++j) {
                if(mGridColBlocked[j]) continue;
                int gridId = i * mTrainData->NumItemBlock() + j;
                int iterationCount = mGridIterationCount[gridId];
                if (iterationCount >= mTrainOption->Iterations() - 1) continue;
                if (iterationCount == minIterationCount) {
                    candidates.push_back(gridId);
                } else if(iterationCount < minIterationCount) {
                    minIterationCount = iterationCount;
                    candidates.clear();
                    candidates.push_back(gridId);
                }
            }
        }
        if (candidates.size() > 0) {
            int chosenGridId = candidates[Random::Instance().Uniform(0, candidates.size())];
            {
                std::unique_lock<std::mutex> lock(mGridBlockedMutex);
                mGridRowBlocked[chosenGridId / mTrainData->NumItemBlock()] = true;
                mGridColBlocked[chosenGridId % mTrainData->NumItemBlock()] = true;
            }
            ++mGridIterationCount[chosenGridId];
            Task* task = new Task();
            task->type = TaskTypeCompute;
            task->gridId = chosenGridId;
            mScheduler->ProducerPutTask(task);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        currIter = Math::MinInRange(mGridIterationCount.begin(), mGridIterationCount.end());
        if (currIter > lastIter) {
            lastIter = currIter;
            if (mTrainOption->MonitorIteration() && currIter % mTrainOption->MonitorIterationStep() == 0) {
                Task* task = new Task();
                task->type = TaskTypeMonitor;
                task->iter = currIter;
                mScheduler->ProducerPutTask(task);
            }
        }
        if (currIter >= mTrainOption->Iterations() - 1) break;
    }
    mScheduler->ProducerDone();
}

void ModelComputationMT::WorkerRun() {
    while(true) {
        Task *task = mScheduler->WorkerGetTask();
        if (task == nullptr) break;
        if (task->type == TaskTypeCompute) {
            SGDOneMatrix(task->gridId);
        } else if (task->type == TaskTypeMonitor) {
            std::unique_lock<std::mutex> lock(mMonitorMutex);
            double dataLoss, regLoss, validateLoss;
            ComputeLoss(&dataLoss, &regLoss, &validateLoss);
            Log::I("recsys", "iteration %d/%d done. \n"
                   "data loss = %lf\t reg loss = %lf\t train loss = %lf\t validate loss = %lf",
                   task->iter, mTrainOption->Iterations(),
                   dataLoss, regLoss, dataLoss + regLoss, validateLoss);
        }
        mScheduler->WorkerPutTask(task);
    }
    mScheduler->WorkerDone();
}

void ModelComputationMT::ConsumerRun() {
    while (true) {
        Task *task = mScheduler->ConsumerGetTask();
        if (task == nullptr) break;
        int gridId = task->gridId;
        {
            std::unique_lock<std::mutex> lock(mGridBlockedMutex);
            mGridRowBlocked[gridId / mTrainOption->NumItemBlock()] = false;
            mGridColBlocked[gridId % mTrainOption->NumItemBlock()] = false;
        }
        delete task;
    }
    mScheduler->ConsumerDone();
}

} //~ namespace SVD

} //~ namespace longan

