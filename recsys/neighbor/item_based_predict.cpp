/*
 * item_based_predict.cpp
 * Created on: Nov 12, 2014
 * Author: chenguangyu
 */

#include "item_based_predict.h"
#include "recsys/base/rating_list_loader.h"
#include "common/logging/logging.h"
#include "common/system/file_util.h"
#include "common/base/algorithm.h"
#include "common/math/math.h"
#include "common/lang/double.h"
#include "common/error.h"
#include <cassert>

namespace longan {

ItemBasedPredict::ItemBasedPredict(const std::string& trainRatingFilepath, const std::string& trainConfigFilepath,
        const std::string& modelFilepath) :
    mTrainRatingFilepath(trainRatingFilepath),
    mConfigFilepath(trainConfigFilepath),
    mModelFilepath(modelFilepath),
    mRatingMatrix(nullptr),
    mModel(nullptr) { }

ItemBasedPredict::~ItemBasedPredict() {
    delete mModel;
    delete mRatingMatrix;
}

void ItemBasedPredict::Init() {
    LoadConfig();
    LoadRatings();
    LoadModel();
}

void ItemBasedPredict::LoadConfig() {
    Log::I("recsys", "ItemBasedPredict::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    std::string content = FileUtil::LoadFileContent(mConfigFilepath);
    Json::Reader reader;
    if (!reader.parse(content, mConfig)) {
        throw LonganFileFormatError();
    }
}

void ItemBasedPredict::LoadRatings() {
    Log::I("recsys", "ItemBasedPredict::LoadRatings()");
    Log::I("recsys", "rating file = " + mTrainRatingFilepath);
    RatingList rlist = RatingListLoader::Load(mTrainRatingFilepath);
    Log::I("recsys", "create rating matrix");
    mRatingMatrix = new RatingMatrixAsUsers<>();
    mRatingMatrix->Init(rlist);
}

void ItemBasedPredict::LoadModel() {
    Log::I("recsys", "ItemBasedPredict::LoadModel()");
    Log::I("recsys", "mdoel file = " + mModelFilepath);
    mModel = new item_based::ModelPredict();
    mModel->Load(mModelFilepath);
}

float ItemBasedPredict::PredictRating(int userId, int itemId) {
	assert(userId >= 0 && userId < mRatingMatrix->NumUser());
	assert(itemId >= 0 && itemId < mRatingMatrix->NumItem());
    const auto& uv = mRatingMatrix->GetUserVector(userId);
    const ItemRating* data1 = uv.Data();
    int size1 = uv.Size();
    const item_based::NeighborItem* data2 = mModel->NeighborBegin(itemId);
    int size2 = mModel->NeighborSize(itemId);
    double numerator = 0.0;
    double denominator = 0.0;
    if (size1 < size2) {
        for (int i = 0; i < size1; ++i) {
            const ItemRating& ir = data1[i];
            int pos = BSearch(ir.ItemId(), data2, size2,
                    [](int lhs, const item_based::NeighborItem& rhs)->int {
                return lhs - rhs.ItemId();
            });
            if (pos >= 0) {
                const item_based::NeighborItem& ni = data2[pos];
                numerator += ni.Similarity() * ir.Rating();
                denominator += Math::Abs(ni.Similarity());
            }
        }
    } else {
        for (int i = 0; i < size2; ++i) {
            const item_based::NeighborItem& ni = data2[i];
            int pos = BSearch(ni.ItemId(), data1, size1,
                    [](int lhs, const ItemRating& rhs)->int {
                return lhs - rhs.ItemId();
            });
            if (pos >= 0) {
                const ItemRating& ir = data1[pos];
                numerator += ni.Similarity() * ir.Rating();
                denominator += Math::Abs(ni.Similarity());
            }
        }
    }
    double predictedRating = numerator / (denominator + Double::EPS);
    return (float)predictedRating;
}

} //~ namespace ycg
