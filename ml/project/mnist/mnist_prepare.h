/*
 * mnist_prepare.h
 * Created on: Jan 24, 2015
 * Author: chenguangyu
 */

#ifndef ML_PROJECT_MNIST_MNIST_PREPARE_H
#define ML_PROJECT_MNIST_MNIST_PREPARE_H

#include <json/json.h>
#include <string>

namespace longan {

class MnistPrepare {
public:
    MnistPrepare(const std::string& inputDirpath, const std::string& configFilepath, const std::string& outputDirpath);
    void Prepare();
private:
    void LoadConfig();
    void PrepareSupervised();
    void PrepareUnsupervised();
    void PrepareImages();
    void PrepareSupervised(const std::string& imageFilename, const std::string& labelFilename, const std::string& datamodelFilename);
    void PrepareUnsupervised(const std::string& imageFilename, const std::string& datamodelFilename);
    void PrepareImages(const std::string& imageFilename, const std::string& outputImageDir);
private:
    const std::string mInputDirpath;
    const std::string mConfigFilepath;
    const std::string mOutputDirpath;
    Json::Value mConfig;
};

} //~ namespace longan

#endif /* ML_PROJECT_MNIST_MNIST_PREPARE_H */
