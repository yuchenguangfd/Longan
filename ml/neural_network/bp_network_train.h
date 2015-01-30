/*
 * bp_network_train.h
 * Created on: Jan 29, 2015
 * Author: chenguangyu
 */

#ifndef ML_NEURAL_NETWORK_BP_NETWORK_TRAIN_H
#define ML_NEURAL_NETWORK_BP_NETWORK_TRAIN_H

#include <json/json.h>
#include <string>

namespace longan {

class SupervisedDatamodel;
class BpNetwork;

class BpNetworkTrain {
public:
    BpNetworkTrain(const std::string& dataTrainFilepath, const std::string& configFilepath,
        const std::string& modelFilepath);
    void Train();
private:
    void LoadConfig();
    void LoadTrainData();
    void InitModel();
    void TrainModel();
    void SaveModel();
    void Cleanup();
private:
    const std::string mDataTrainFilepath;
    const std::string mConfigFilepath;
    const std::string mModelFilepath;
    Json::Value mConfig;
    SupervisedDatamodel *mTrainData;
    BpNetwork *mModel;
};


} //~ namespace longan

#endif /* ML_NEURAL_NETWORK_BP_NETWORK_TRAIN_H */
