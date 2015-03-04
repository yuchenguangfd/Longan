/*
 * svd_predict_unittest.cpp
 * Created on: Jan 8, 2015
 * Author: chenguangyu
 */

#include "svd_predict.h"
#include "common/system/file_util.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(SVDPredictTest, PredictRatingOK) {
    class ModelTrainMock : public SVD::ModelTrain {
    public:
        using ModelTrain::ModelTrain;
        void InitMockData() {
            float data1[] = {1, 2, 3};
            float data2[] = {4, 5, 6};
            float data3[] = {7, 8, 9};
            float data4[] = {10,11,12};
            mNumUser = 2;
            mNumItem = 2;
            mUserFeatures.resize(2);
            mUserFeatures[0] = Vector<float>(data1, data1 + 3);
            mUserFeatures[1] = Vector<float>(data2, data2 + 3);
            mItemFeatures.resize(2);
            mItemFeatures[0] = Vector<float>(data3, data3 + 3);
            mItemFeatures[1] = Vector<float>(data4, data4 + 3);
            mUserBiases.resize(2);
            mUserBiases[0] = 100;
            mUserBiases[1] = 200;
            mItemBiases.resize(2);
            mItemBiases[0] = 300;
            mItemBiases[1] = 400;
            mRatingAverage = 1000;
        }
    };
    class SVDPredictMock : public SVDPredict {
    public:
        SVDPredictMock(const std::string& modelFilename, SVD::Parameter *parameter, SVD::TrainOption *option) :
            SVDPredict("", "", modelFilename) {
            mParameter = parameter;
            mTrainOption = option;
            mModel = new SVD::Model(mParameter);
            mModel->Load(mModelFilepath);
        }
    };
    Json::Value config;
    config["parameter"]["dim"] = 3;
    config["parameter"]["lambdaUserFeature"] = 0.01;
    config["parameter"]["lambdaItemFeature"] = 0.02;
    config["parameter"]["lambdaUserBias"] = 0.001;
    config["parameter"]["lambdaItemBias"] = 0.002;
    config["trainOption"]["iterations"] = 1;
    config["trainOption"]["learningRate"] = 0.1;
    config["trainOption"]["useRatingAverage"] = true;
    config["trainOption"]["numUserBlock"] = 2;
    config["trainOption"]["numItemBlock"] = 2;
    SVD::Parameter parameter(config["parameter"]);
    ModelTrainMock modelTrain(&parameter, 2, 2, 0.0);
    modelTrain.InitMockData();
    modelTrain.Save("tmp.dat");
    SVD::TrainOption option(config["trainOption"]);
    SVDPredictMock predict("tmp.dat", &parameter, &option);
    EXPECT_FLOAT_EQ(1450, predict.PredictRating(0, 0));
    EXPECT_FLOAT_EQ(1568, predict.PredictRating(0, 1));
    EXPECT_FLOAT_EQ(1622, predict.PredictRating(1, 0));
    EXPECT_FLOAT_EQ(1767, predict.PredictRating(1, 1));
    FileUtil::Delete("tmp.dat");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
