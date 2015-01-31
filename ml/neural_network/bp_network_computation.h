/*
 * bp_network_computation.h
 * Created on: Jan 29, 2015
 * Author: chenguangyu
 */

#ifndef ML_NEURAL_NETWORK_BP_NETWORK_COMPUTATION_H
#define ML_NEURAL_NETWORK_BP_NETWORK_COMPUTATION_H

#include "common/math/vector.h"
#include "common/math/matrix.h"
#include <vector>

namespace longan {

class BpNetwork;
class BpNetworkTrainOption;
class SupervisedDatamodel;

class BpNetworkComputation {
public:
    virtual ~BpNetworkComputation();
    virtual void Train(BpNetwork *bpNetwork, const BpNetworkTrainOption *trainOption,
            const SupervisedDatamodel *datamodel) = 0;
protected:
    void RandomInit();
protected:
    const BpNetworkTrainOption *mTrainOption;
    const SupervisedDatamodel *mDatamodel;
    BpNetwork *mNetwork;
};

class BpNetworkComputationSimple : public BpNetworkComputation {
public:
    virtual void Train(BpNetwork *network, const BpNetworkTrainOption *trainOption,
              const SupervisedDatamodel *datamodel) override;
private:
    void InitResource();
    void Forward();
    void Backward();
    void ComputeGradient();
    void AdjustNetwork();
    void ComputeTotalCost();
private:
    std::vector<int> mSampleOrder;
    std::vector<Vector64F> mActivations;
    Vector64F mTarget;
    std::vector<Vector64F> mDeltas;
    std::vector<Matrix64F> mWeightGradients;
    std::vector<Vector64F> mBiasGradients;
    std::vector<Matrix64F> mWeightAdjust;
    std::vector<Vector64F> mBiasAdjust;
    double mTotalCost;
};

class BpNetworkComputationST : public BpNetworkComputation {
public:
    virtual void Train(BpNetwork *network, const BpNetworkTrainOption *trainOption,
              const SupervisedDatamodel *datamodel) override;
};

class BpNetworkComputationMT : public BpNetworkComputation {
public:
    virtual void Train(BpNetwork *network, const BpNetworkTrainOption *trainOption,
              const SupervisedDatamodel *datamodel) override;
private:

};

} //~ namespace longan

#endif /* ML_NEURAL_NETWORK_BP_NETWORK_COMPUTATION_H */
