/*
 * mnist_prepare.cpp
 * Created on: Jan 24, 2015
 * Author: chenguangyu
 */

#include "mnist_prepare.h"
#include "ml/base/datamodel.h"
#include "cv/base/image_opencv.h"
#include "common/lang/integer.h"
#include "common/lang/binary_input_stream.h"
#include "common/system/file_util.h"
#include "common/logging/logging.h"
#include "common/error.h"

namespace longan {

MnistPrepare::MnistPrepare(const std::string& inputDirpath, const std::string& configFilepath,
    const std::string& outputDirpath) :
    mInputDirpath(inputDirpath),
    mConfigFilepath(configFilepath),
    mOutputDirpath(outputDirpath) { }

void MnistPrepare::Prepare() {
    LoadConfig();
    if (mConfig["datasetType"].asString() == "supervised") {
        PrepareSupervised();
    } else if (mConfig["datasetType"].asString() == "unsupervised") {
        PrepareUnsupervised();
    } else {
        throw LonganConfigError("No Such datasetType");
    }
    if (mConfig["outputImage"].asBool()) {
        PrepareImages();
    }
}

void MnistPrepare::LoadConfig() {
    Log::I("ml", "MnistPrepare::LoadConfig()");
    Log::I("ml", "config file = " + mConfigFilepath);
    std::string content = FileUtil::LoadFileContent(mConfigFilepath);
    Json::Reader reader;
    if (!reader.parse(content, mConfig)) {
        throw LonganFileFormatError();
    }
}

void MnistPrepare::PrepareSupervised() {
    PrepareSupervised(
            mInputDirpath + "/train-images.idx3-ubyte",
            mInputDirpath + "/train-labels.idx1-ubyte",
            mOutputDirpath + "/data_train.bin");
    PrepareSupervised(
            mInputDirpath + "/t10k-images.idx3-ubyte",
            mInputDirpath + "/t10k-labels.idx1-ubyte",
            mOutputDirpath + "/data_test.bin");
}

void MnistPrepare::PrepareUnsupervised() {
    PrepareUnsupervised(
            mInputDirpath + "/train-images.idx3-ubyte",
            mOutputDirpath + "/data_train.bin");
    PrepareUnsupervised(
            mInputDirpath + "/t10k-images.idx3-ubyte",
            mOutputDirpath + "/data_test.bin");
}

void MnistPrepare::PrepareImages() {
    PrepareImages(
            mInputDirpath + "/train-images.idx3-ubyte",
            mOutputDirpath + "/images/train/");
    PrepareImages(
            mInputDirpath + "/t10k-images.idx3-ubyte",
            mOutputDirpath + "/images/test/");
}

void MnistPrepare::PrepareSupervised(const std::string& imageFilename, const std::string& labelFilename,
    const std::string& datamodelFilename) {
    Log::I("ml", "MnistPrepare::PrepareSupervised(), imageFilename=%s, "
           "labelFilename=%s, datamodelFilename=%s", imageFilename.c_str(), labelFilename.c_str(),
           datamodelFilename.c_str());
    BinaryInputStream imageBis(imageFilename);
    BinaryInputStream labelBis(labelFilename);
    int magic1, magic2;
    imageBis >> magic1;
    labelBis >> magic2;
    if (Integer::ReverseByteOrder(magic1) != 0x00000803 || Integer::ReverseByteOrder(magic2) != 0x00000801) {
        throw LonganFileFormatError();
    }
    int numSample, rows, cols;
    imageBis >> numSample; numSample = Integer::ReverseByteOrder(numSample);
    imageBis >> rows; rows = Integer::ReverseByteOrder(rows);
    imageBis >> cols; cols = Integer::ReverseByteOrder(cols);
    int featureDim = rows * cols;
    int numClass = 10;
    int numSample2;
    labelBis >> numSample2; numSample2 = Integer::ReverseByteOrder(numSample2);
    assert(numSample == numSample2);
    ClassificationDatamodel model(featureDim, numClass, numSample);
    Vector64F feature(featureDim);
    for (int i = 0; i < numSample; ++i) {
       for (int j = 0; j < featureDim; ++j) {
           uint8 pixel;
           imageBis >> pixel;
           feature[j] = pixel / 255.0;
       }
       uint8 label;
       labelBis >> label;
       model.AddSample(feature, label);
    }
    ClassificationDatamodel::WriteToBinaryFile(model, datamodelFilename);
}

void MnistPrepare::PrepareUnsupervised(const std::string& imageFilename, const std::string& datamodelFilename) {
    Log::I("ml", "MnistPrepare::PrepareUnsupervised(), imageFilename=%s, "
           "datamodelFilename=%s", imageFilename.c_str(), datamodelFilename.c_str());
    BinaryInputStream imageBis(imageFilename);
    int magic;
    imageBis >> magic;
    if (Integer::ReverseByteOrder(magic) != 0x00000803) {
        throw LonganFileFormatError();
    }
    int numSample, rows, cols;
    imageBis >> numSample; numSample = Integer::ReverseByteOrder(numSample);
    imageBis >> rows; rows = Integer::ReverseByteOrder(rows);
    imageBis >> cols; cols = Integer::ReverseByteOrder(cols);
    int featureDim = rows * cols;
    int numClass = 10;
    UnsupervisedDatamodel model(featureDim, numSample);
    Vector64F feature(featureDim);
    for (int i = 0; i < numSample; ++i) {
       for (int j = 0; j < featureDim; ++j) {
           uint8 pixel;
           imageBis >> pixel;
           feature[j] = pixel / 255.0;
       }
       model.AddSample(feature);
    }
    UnsupervisedDatamodel::WriteToBinaryFile(model, datamodelFilename);
}

void MnistPrepare::PrepareImages(const std::string& imageFilename, const std::string& outputImageDir) {
    Log::I("ml", "MnistPrepare::PrepareImages(), imageFilename=%s, "
           "outputImageDir=%s", imageFilename.c_str(), outputImageDir.c_str());
    BinaryInputStream imageBis(imageFilename);
    int magic;
    imageBis >> magic;
    if (Integer::ReverseByteOrder(magic) != 0x00000803) {
        throw LonganFileFormatError();
    }
    int numSample, rows, cols;
    imageBis >> numSample; numSample = Integer::ReverseByteOrder(numSample);
    imageBis >> rows; rows = Integer::ReverseByteOrder(rows);
    imageBis >> cols; cols = Integer::ReverseByteOrder(cols);
    for (int i = 0; i < numSample; ++i) {
        if (i % 1000 == 0) {
            Log::Console("ml", "generating images(%d/%d)...", i, numSample);
        }
        ImageOpenCV8U image(cols, rows);
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                uint8 pixel;
                imageBis >> pixel;
                image.SetPixel(c, r, pixel);
            }
        }
        image.Save(outputImageDir + Integer::ToString(i, 5) + ".png");
    }
}

} //~ namespace longan
