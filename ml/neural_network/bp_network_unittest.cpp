/*
 * bp_network_unittest.h
 * Created on: Jan 31, 2015
 * Author: chenguangyu
 */

#include "bp_network.h"
#include "common/util/random.h"
#include "common/math/math.h"
#include "common/error.h"
#include <json/json.h>
#include <gtest/gtest.h>
#include <glog/logging.h>

using namespace std;
using namespace longan;

TEST(BpNetworkTest, ModelXorOK) {
    Json::Value archConfig;
    archConfig["numLayer"] = 3;
    archConfig["numLayerUnits"].append(2);
    archConfig["numLayerUnits"].append(2);
    archConfig["numLayerUnits"].append(1);
    BpNetworkArchitecture arch(archConfig);
    BpNetwork network(arch);

    SupervisedDatamodel datamodel(2, 1);
    Vector64F feature1(2), feature2(2), feature3(2), feature4(2);
    Vector64F target1(1), target2(1), target3(1), target4(1);
    feature1[0] = 0.0; feature1[1] = 0.0; target1[0] = 0.0;
    feature1[0] = 0.0; feature1[1] = 1.0; target1[0] = 1.0;
    feature1[0] = 1.0; feature1[1] = 0.0; target1[0] = 1.0;
    feature1[0] = 1.0; feature1[1] = 1.0; target1[0] = 0.0;
    datamodel.AddSample(feature1, target1);
    datamodel.AddSample(feature2, target2);
    datamodel.AddSample(feature3, target3);
    datamodel.AddSample(feature4, target4);

    Json::Value trainOptionConfig;
    trainOptionConfig["isRandomInit"] = true;
    trainOptionConfig["lambda"] = 0.0001;
    trainOptionConfig["learningRate"] = 0.1;
    trainOptionConfig["iterations"] = 500;
    trainOptionConfig["accelerate"] = false;
    BpNetworkTrainOption trainOpt(trainOptionConfig);
    network.Train(&trainOpt, &datamodel);
    for (int i = 0; i < datamodel.NumSample(); ++i) {
        const Vector64F& input = datamodel.Feature(i);
        const Vector64F& target = datamodel.Target(i);
        Vector64F output = network.Predict(input);
        int ans1 = (target[0] >= 0.5) ? 1 : 0;
        int ans2 = (output[0] >= 0.5) ? 1 : 0;
        ASSERT_TRUE(ans1 == ans2);
    }

    trainOptionConfig["accelerate"] = true;
    trainOptionConfig["numThread"] = 1;
    BpNetworkTrainOption trainOpt2(trainOptionConfig);
    network.Train(&trainOpt2, &datamodel);
    for (int i = 0; i < datamodel.NumSample(); ++i) {
        const Vector64F& input = datamodel.Feature(i);
        const Vector64F& target = datamodel.Target(i);
        Vector64F output = network.Predict(input);
        int ans1 = (target[0] >= 0.5) ? 1 : 0;
        int ans2 = (output[0] >= 0.5) ? 1 : 0;
        ASSERT_TRUE(ans1 == ans2);
    }

    trainOptionConfig["accelerate"] = true;
    trainOptionConfig["numThread"] = 8;
    BpNetworkTrainOption trainOpt3(trainOptionConfig);
    network.Train(&trainOpt3, &datamodel);
    for (int i = 0; i < datamodel.NumSample(); ++i) {
        const Vector64F& input = datamodel.Feature(i);
        const Vector64F& target = datamodel.Target(i);
        Vector64F output = network.Predict(input);
        int ans1 = (target[0] >= 0.5) ? 1 : 0;
        int ans2 = (output[0] >= 0.5) ? 1 : 0;
        ASSERT_TRUE(ans1 == ans2);
    }
}

void BpNetworkTest_ModelIntegerSignAndParityOK_GenerateSample(int value, Vector64F& feature, Vector64F& target) {
    feature = Vector64F(32);
    for (int k = 0; k < 32; ++k) {
       feature[k] = static_cast<double>((value >> k) & 0x1);
    }
    target = Vector64F(4, true, 0.0);
    if (value >= 0 && (value & 1) == 1) {
       target[0] = 1.0;
    } else if (value >= 0 && (value & 1) == 0) {
       target[1] = 1.0;
    } else if (value < 0 && (value & 1) == 1) {
       target[2] = 1.0;
    } else if (value < 0 && (value & 1) == 0) {
       target[3] = 1.0;
    }
}


TEST(BpNetworkTest, ModelIntegerSignAndParityOK) {
    Json::Value archConfig;
    archConfig["numLayer"] = 4;
    archConfig["numLayerUnits"].append(32);
    archConfig["numLayerUnits"].append(20);
    archConfig["numLayerUnits"].append(10);
    archConfig["numLayerUnits"].append(4);
    BpNetworkArchitecture arch(archConfig);
    BpNetwork network(arch);

    SupervisedDatamodel datamodel(32, 4);
    int numSample = 1024;
    for (int i = 0; i != numSample; i++) {
        Vector64F feature, target;
        int value = Random::Instance().NextInt();
        BpNetworkTest_ModelIntegerSignAndParityOK_GenerateSample(value, feature, target);
        datamodel.AddSample(feature, target);
    }

    Json::Value trainOptionConfig;
    trainOptionConfig["isRandomInit"] = true;
    trainOptionConfig["lambda"] = 0.00001;
    trainOptionConfig["learningRate"] = 0.01;
    trainOptionConfig["iterations"] = 1000;
    trainOptionConfig["accelerate"] = false;
    trainOptionConfig["iterationCheckStep"] = 10;
    BpNetworkTrainOption trainOpt(trainOptionConfig);
    network.Train(&trainOpt, &datamodel);

    int numTestSample = 20000;
    int countError = 0;
    for (int i = 0; i < numTestSample; ++i) {
        int value = Random::Instance().NextInt();
        Vector64F feature, target;
        BpNetworkTest_ModelIntegerSignAndParityOK_GenerateSample(value, feature, target);
        Vector64F output = network.Predict(feature);
        int idx1 = Math::MaxIndex(target.Data(), target.Size());
        int idx2 = Math::MaxIndex(output.Data(), output.Size());
        if (idx1 != idx2) ++countError;
    }
    double errorRate = static_cast<double>(countError) / numTestSample;
    ASSERT_LE(errorRate, 0.0001);

    trainOptionConfig["accelerate"] = true;
    trainOptionConfig["useOpenMP"] = true;
    trainOptionConfig["numThread"] = 2;
    BpNetworkTrainOption trainOpt2(trainOptionConfig);
    network.Train(&trainOpt2, &datamodel);

    countError = 0;
    for (int i = 0; i < numTestSample; ++i) {
        int value = Random::Instance().NextInt();
        Vector64F feature, target;
        BpNetworkTest_ModelIntegerSignAndParityOK_GenerateSample(value, feature, target);
        Vector64F output = network.Predict(feature);
        int idx1 = Math::MaxIndex(target.Data(), target.Size());
        int idx2 = Math::MaxIndex(output.Data(), output.Size());
        if (idx1 != idx2) ++countError;
    }
    errorRate = static_cast<double>(countError) / numTestSample;
    ASSERT_LE(errorRate, 0.0001);

    trainOptionConfig["accelerate"] = true;
    trainOptionConfig["useOpenMP"] = false;
    trainOptionConfig["numThread"] = 4;
    BpNetworkTrainOption trainOpt3(trainOptionConfig);
    network.Train(&trainOpt3, &datamodel);

    countError = 0;
    for (int i = 0; i < numTestSample; ++i) {
        int value = Random::Instance().NextInt();
        Vector64F feature, target;
        BpNetworkTest_ModelIntegerSignAndParityOK_GenerateSample(value, feature, target);
        Vector64F output = network.Predict(feature);
        int idx1 = Math::MaxIndex(target.Data(), target.Size());
        int idx2 = Math::MaxIndex(output.Data(), output.Size());
        if (idx1 != idx2) ++countError;
    }
    errorRate = static_cast<double>(countError) / numTestSample;
    ASSERT_LE(errorRate, 0.0001);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
