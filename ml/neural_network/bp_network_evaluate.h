/*
 * bp_network_evaluate.h
 * Created on: Jan 29, 2015
 * Author: chenguangyu
 */

#ifndef ML_NEURAL_NETWORK_BP_NETWORK_EVALUATE_H
#define ML_NEURAL_NETWORK_BP_NETWORK_EVALUATE_H

#include <json/json.h>

namespace longan {

class SupervisedDatamodel;
class BpNetwork;

class BpNetworkEvaluate {
public:
    BpNetworkEvaluate(const std::string& dataTrainFilepath, const std::string& configFilepath,
        const std::string& modelFilepath, const std::string& dataTestFilepath, const std::string& resultFilepath);
    void Evaluate();
private:
    void LoadConfig();
    void LoadTestData();
    void LoadModel();
    void EvaluateAccuracy();
    void WriteResult();
    void Cleanup();
private:
    const std::string mConfigFilepath;
    const std::string mModelFilepath;
    const std::string mDataTestFilepath;
    const std::string mResultFilepath;
    Json::Value mConfig;
    SupervisedDatamodel *mTestData;
    BpNetwork *mModel;
    double mAccuracy;
    double mErrorRate;
};

} //~ namespace longan

#endif /* ML_NEURAL_NETWORK_BP_NETWORK_EVALUATE_H */
