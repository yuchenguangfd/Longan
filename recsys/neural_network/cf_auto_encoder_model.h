/*
 * cf_auto_encoder.h
 * Created on: Feb 3, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_H
#define RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_H

#include "cf_auto_encoder_util.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "recsys/base/rating_matrix_as_items.h"
#include "common/math/vector.h"
#include "common/math/matrix.h"
#include "common/lang/defines.h"

namespace longan {

namespace CFAE {

class ModelComputation;
class TrainSparseLayerDelegate;
class TrainSparseLayerDelegateST;
class TrainSparseLayerDelegateMT;
class EncodeSamplesDelegateST;

class Model {
public:
    Model(const Parameter *param, int numInputUnit, int numSample);
    void RandomInit();
    void Save(const std::string& filename) const;
    void Load(const std::string& filename);
    int NumSample() const { return mNumSample; }
    const Vector64F& Code(int sampleId) const { return mCodes[sampleId]; }
    float Reconstruct(int userId, int itemId) const;
    friend class ModelComputation;
    friend class TrainSparseLayerDelegate;
    friend class TrainSparseLayerDelegateST;
    friend class TrainSparseLayerDelegateMT;
    friend class EncodeSamplesDelegateST;
private:
    const Parameter *mParameter;
    int mNumInputUnit;
    int mNumSample;
    int mCodeLength;
    std::vector<Matrix64F> mEncodeWeights;
    std::vector<Vector64F> mEncodeBiases;
    std::vector<Matrix64F> mDecodeWeights;
    std::vector<Vector64F> mDecodeBiases;
    std::vector<Vector64F> mCodes;
    DISALLOW_COPY_AND_ASSIGN(Model);
};

} //~ namespace CFAE

} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_H */
