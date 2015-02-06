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

class CFAutoEncoder {
public:
    CFAutoEncoder();
    CFAutoEncoder(const CFAE::Architecture& arch);
    Vector64F Reconstruct(const UserVector<>& input) const;
    Vector64F Reconstruct(int userId) const;
    Vector64F Reconstruct(int userId, int itemId) const;
    Vector64F Reconstruct(const UserVector<>& input, int itemId) const;
    const Vector64F& GetCode(int userId) const { return mCodes[userId]; }
    void Train(const CFAE::Parameter *param, const CFAE::TrainOption *option,
            RatingMatrixAsUsers<> *data);
//    void Train(const CFAE::TrainOption *option, RatingMatrixAsItems<> *data);
    void Load(const std::string& filename);
    void Save(const std::string& filename) const;
    friend class CFAutoEncoderComputation;
    friend class CFAutoEncoderComputationST;
    friend class CFAutoEncoderComputationMT;
private:
    Vector64F Encode(const UserVector<>& input) const;
    Vector64F Decode(const Vector64F& code) const;
private:
    int mNumPossibleRating;
    int mNumInputUnit;
    int mNumHiddenUnit;
    int mNumOutputUnit;
    Matrix64F mWeight1;
    Vector64F mBias1;
    Matrix64F mWeight2;
    Vector64F mBias2;
    RatingMatrixAsUsers<> *mTrainData;
    std::vector<Vector64F> mCodes;
    DISALLOW_COPY_AND_ASSIGN(CFAutoEncoder);
};

} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_H */
