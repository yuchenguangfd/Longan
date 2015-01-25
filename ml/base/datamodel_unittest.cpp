/*
 * datamodel_unittest.cpp
 * Created on: Jan 21, 2015
 * Author: chenguangyu
 */

#include "datamodel.h"
#include "common/system/file_util.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(UnsupervisedDatamodelTest, LoadAndWriteOK) {
    int numSample = 1024;
    int featureDim = 31;
    UnsupervisedDatamodel model(featureDim);
    for (int i = 0; i < numSample; ++i) {
        model.AddSample(Vector64F::Rand(featureDim));
    }
    ASSERT_NO_THROW(UnsupervisedDatamodel::WriteToTextFile(model, "temp.txt"));
    ASSERT_NO_THROW(UnsupervisedDatamodel::WriteToBinaryFile(model, "temp.bin"));
    ASSERT_NO_THROW(UnsupervisedDatamodel m1 = UnsupervisedDatamodel::LoadFromTextFile("temp.txt"));
    ASSERT_NO_THROW(UnsupervisedDatamodel m2 = UnsupervisedDatamodel::LoadFromBinaryFile("temp.bin"));
    FileUtil::Delete("temp.txt");
    FileUtil::Delete("temp.bin");
}

TEST(ClassificationDatamodelTest, LoadAndWriteOK) {
    int numSample = 1024;
    int featureDim = 31;
    int numClass = 10;
    ClassificationDatamodel model(featureDim, numClass, numSample);
    for (int i = 0; i < numSample; ++i) {
        model.AddSample(Vector64F::Rand(featureDim), Random::Instance().Uniform(0, numClass));
    }
    ASSERT_NO_THROW(ClassificationDatamodel::WriteToTextFile(model, "temp.txt"));
    ASSERT_NO_THROW(ClassificationDatamodel::WriteToBinaryFile(model, "temp.bin"));
    ASSERT_NO_THROW(ClassificationDatamodel m1 = ClassificationDatamodel::LoadFromTextFile("temp.txt"));
    ASSERT_NO_THROW(ClassificationDatamodel m2 = ClassificationDatamodel::LoadFromBinaryFile("temp.bin"));
    FileUtil::Delete("temp.txt");
    FileUtil::Delete("temp.bin");
}

TEST(RegressionDatamodelTest, LoadAndWriteOK) {
    int numSample = 1024;
    int featureDim = 31;
    RegressionDatamodel model(featureDim, numSample);
    for (int i = 0; i < numSample; ++i) {
        model.AddSample(Vector64F::Rand(featureDim), Random::Instance().Uniform(-1.2, +3.4));
    }
    ASSERT_NO_THROW(RegressionDatamodel::WriteToTextFile(model, "temp.txt"));
    ASSERT_NO_THROW(RegressionDatamodel::WriteToBinaryFile(model, "temp.bin"));
    ASSERT_NO_THROW(RegressionDatamodel m1 = RegressionDatamodel::LoadFromTextFile("temp.txt"));
    ASSERT_NO_THROW(RegressionDatamodel m2 = RegressionDatamodel::LoadFromBinaryFile("temp.bin"));
    FileUtil::Delete("temp.txt");
    FileUtil::Delete("temp.bin");
}

TEST(SupervisedDatamodelTest, LoadAndWriteOK) {
    int numSample = 1024;
    int featureDim = 15;
    int numClass = 10;
    ClassificationDatamodel cmodel(featureDim, numClass, numSample);
    for (int i = 0; i < numSample; ++i) {
        cmodel.AddSample(Vector64F::Rand(featureDim), Random::Instance().Uniform(0, numClass));
    }
    SupervisedDatamodel smodel(std::move(cmodel));
    ASSERT_NO_THROW(SupervisedDatamodel::WriteToTextFile(smodel, "temp.txt"));
    ASSERT_NO_THROW(SupervisedDatamodel::WriteToBinaryFile(smodel, "temp.bin"));
    ASSERT_NO_THROW(SupervisedDatamodel m1 = SupervisedDatamodel::LoadFromTextFile("temp.txt"));
    ASSERT_NO_THROW(SupervisedDatamodel m2 = SupervisedDatamodel::LoadFromBinaryFile("temp.bin"));
    FileUtil::Delete("temp.txt");
    FileUtil::Delete("temp.bin");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

