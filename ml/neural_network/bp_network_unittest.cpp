#include "bp_network.h"
#include "common/util/random.h"
#include "common/math/math.h"
#include "common/error.h"
#include <json/json.h>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <iostream>

using namespace std;
using namespace longan;

/*TEST(BpNetwork, Xor) {
    LOG(INFO) << "TEST(BpNetwork, Xor)";
    LOG(INFO) << "config bpnetwork.";
    Json::Value bpConfig;
    bpConfig["architecture"].append(2);
    bpConfig["architecture"].append(2);
    bpConfig["architecture"].append(1);
    BpNetwork bp(bpConfig);
    LOG(INFO) << "config done.\n" << bpConfig.toStyledString();

    LOG(INFO) << "generate trainning data.";
    SupervisedDataModel datamodel(2, 1);
    Vector feature1(2), feature2(2), feature3(2), feature4(2);
    Vector target1(1), target2(1), target3(1), target4(1);
    feature1[0] = 0.0; feature1[1] = 0.0; target1[0] = 0.0;
    feature1[0] = 0.0; feature1[1] = 1.0; target1[0] = 1.0;
    feature1[0] = 1.0; feature1[1] = 0.0; target1[0] = 1.0;
    feature1[0] = 1.0; feature1[1] = 1.0; target1[0] = 0.0;
    datamodel.AddSample(feature1, target1);
    datamodel.AddSample(feature2, target2);
    datamodel.AddSample(feature3, target3);
    datamodel.AddSample(feature4, target4);
    LOG(INFO) << "num of trainning sample = " << datamodel.NumSamples();
    LOG(INFO) << "generate trainning data done.";

    LOG(INFO) << "start training bp network.";
    Json::Value trainConfig;
    trainConfig["trainMethod"] = "SGD";
    trainConfig["lambda"] = 0.0001;
    trainConfig["learningRate"] = 0.1;
    trainConfig["momentum"] = 0.3;
    trainConfig["iterations"] = 200;
    trainConfig["iterationCheckStep"] = 20;
    LOG(INFO) << "train config done. \n" << trainConfig.toStyledString();
    bp.Train(trainConfig, datamodel);
    LOG(INFO) << "training done.";

    LOG(INFO) << "start testing bp network";
    int countError = 0;
    for (int i = 0; i < datamodel.Size(); ++i) {
        Vector input = datamodel.Feature(i);
        Vector target = datamodel.Target(i);
        Vector output;
        bp.PredictActivation(input, output);
        int ans1 = (target[0] >= 0.5) ? 1 : 0;
        int ans2 = (output[0] >= 0.5) ? 1 : 0;
        if (ans1 != ans2) {
            LOG(INFO) << "predict error input: " << input
                      << " target: " << target
                      << " output: " << output;
            ++countError;
        }
    }
    LOG(INFO) << "done test. num of errors = " << countError;
    EXPECT_LE(countError, 0);
}

int IntegerSignAndParity_GenerateSample(int value, Vector& feature, Vector& target) {
    feature = Vector(32, false);
    for (int k = 0; k < 32; ++k) {
       feature[k] = static_cast<double>((value >> k) & 0x1);
    }
    target = Vector(4, 0.0);
    if (value >= 0 && (value & 1) == 1) {
       target[0] = 1.0;
    } else if (value >= 0 && (value & 1) == 0) {
       target[1] = 1.0;
    } else if (value < 0 && (value & 1) == 1) {
       target[2] = 1.0;
    } else if (value < 0 && (value & 1) == 0) {
       target[3] = 1.0;
    }
    return LONGAN_SUCC;
}

TEST(BpNetwork, IntegerSignAndParity) {
    LOG(INFO) << "TEST(BpNetwork, IntegerSignAndParity)";
    LOG(INFO) << "config bpnetwork.";
    Json::Value bpConfig;
    bpConfig["architecture"].append(32);
    bpConfig["architecture"].append(6);
    bpConfig["architecture"].append(4);
    BpNetwork bp(bpConfig);
    LOG(INFO) << "config done.\n" << bpConfig.toStyledString();

    LOG(INFO) << "generate training data.";
    SupervisedDataModel datamodel(32, 4);
    int numSample = 1024;
    for (int i = 0; i != numSample; i++) {
        Vector feature, target;
        int value = Random::Instance().NextInt();
        IntegerSignAndParity_GenerateSample(value, feature, target);
        datamodel.AddSample(feature, target);
    }
    LOG(INFO) << "num of trainning sample = " << numSample;
    LOG(INFO) << "generate trainning data done.";

    LOG(INFO) << "start training bp network.";
    Json::Value trainConfig;
    trainConfig["trainMethod"] = "SGD";
    trainConfig["lambda"] = 0.00001;
    trainConfig["learningRate"] = 0.01;
    trainConfig["momentum"] = 0.5;
    trainConfig["iterations"] = 2000;
    LOG(INFO) << "train config done. \n" << trainConfig.toStyledString();
    bp.Train(trainConfig, datamodel);
    LOG(INFO) << "training done.";

    LOG(INFO) << "start testing bp network";
    int numTestSample = 20000;
    int countError = 0;
    LOG(INFO) << "num of test sample = " << numTestSample;
    for (int i = 0; i < numTestSample; ++i) {
        int value = Random::Instance().NextInt();
        Vector feature, target;
        IntegerSignAndParity_GenerateSample(value, feature, target);
        int idx1 = Math::MaxIndex(target.Data(), target.Size());
        int idx2 = bp.PredictLabel(feature);
        if (idx1 != idx2) {
            LOG(INFO) << "predict error, output = " << idx2 << ", target = " << idx1;
            ++countError;
        }
    }
    double errorRate = static_cast<double>(countError) / numTestSample;
    LOG(INFO) << "done test. error rate = " << countError << "/" << numTestSample
              << " = " << errorRate;
    EXPECT_LE(errorRate, 0.001);
}*/

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    google::InitGoogleLogging(argv[0]);
    return RUN_ALL_TESTS();
}
