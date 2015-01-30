/*
 * bp_network.h
 * Created on: Jan 26, 2015
 * Author: chenguangyu
 */

#ifndef ML_NEURAL_NETWORK_BP_NETWORK_H
#define ML_NEURAL_NETWORK_BP_NETWORK_H

#include "bp_network_util.h"
#include "ml/base/datamodel.h"
#include "common/math/vector.h"
#include "common/math/matrix.h"
#include "common/lang/defines.h"
#include <json/json.h>
#include <vector>

namespace longan {

/* Multi-Layer Neural network, trained by BackProprogation algorithm */
class BpNetwork {
public:
    BpNetwork(const BpNetworkArchitecture& arch);
    virtual ~BpNetwork();
    Vector64F Predict(const Vector64F& input);
    void Train(const BpNetworkTrainOption *trainOption, const SupervisedDatamodel *datamodel);
    void Load(const std::string& filename);
    void Save(const std::string& filename) const;
    friend class BpNetworkComputation;
    friend class BpNetworkComputationSimple;
    friend class BpNetworkComputationMT;
private:
    BpNetworkArchitecture mArchitecture;
    int mNumLayer;
    std::vector<Matrix64F> mWeights;
    std::vector<Vector64F> mBiases;
    DISALLOW_COPY_AND_ASSIGN(BpNetwork);
};

} //~ namespace longan

#endif /* ML_NEURAL_NETWORK_BP_NETWORK_H */
