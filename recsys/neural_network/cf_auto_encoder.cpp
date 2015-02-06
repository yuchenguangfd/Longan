/*
 * cf_auto_encoder.cpp
 * Created on: Feb 3, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder.h"
#include "cf_auto_encoder_computation.h"
#include "common/math/math.h"
#include "common/lang/binary_input_stream.h"
#include "common/lang/binary_output_stream.h"
#include "common/logging/logging.h"

namespace longan {

CFAutoEncoder::CFAutoEncoder() :
    mNumPossibleRating(0),
    mNumInputUnit(0),
    mNumHiddenUnit(0),
    mNumOutputUnit(0),
    mTrainData(nullptr) { }

CFAutoEncoder::CFAutoEncoder(const CFAE::Architecture& arch) :
    mNumPossibleRating(0),
    mNumInputUnit(0),
    mNumHiddenUnit(arch.NumHiddenUnit()),
    mNumOutputUnit(0),
    mTrainData(nullptr) { }

Vector64F CFAutoEncoder::Reconstruct(const UserVector<>& input) const {
    return std::move(Decode(Encode(input)));
}

Vector64F CFAutoEncoder::Reconstruct(int userId) const {
    assert(0 <= userId && userId < mCodes.size());
    return std::move(Decode(mCodes[userId]));
}

Vector64F CFAutoEncoder::Reconstruct(int userId, int itemId) const {
    assert(0 <= userId && userId < mCodes.size());
    const Vector64F& code = mCodes[userId];
    Vector64F output(mNumPossibleRating);
    for (int i = itemId * mNumPossibleRating, ii = 0; ii < mNumPossibleRating; ++i, ++ii) {
        double sum = mBias2[i];
        for (int j = 0; j < mNumHiddenUnit; ++j) {
            sum += mWeight2[i][j] * code[j];
        }
        output[ii] = Math::Sigmoid(sum);
    }
    return std::move(output);
}

Vector64F CFAutoEncoder::Reconstruct(const UserVector<>& input, int itemId) const {
    Vector64F code(mNumHiddenUnit);
    const ItemRating *data = input.Data();
    int size = input.Size();
    for (int i = 0; i < mNumHiddenUnit; ++i) {
        double sum = mBias1[i];
        for (int j = 0; j < size; ++j) {
            int jj = data[j].ItemId() * mNumPossibleRating
                    + static_cast<int>(data[j].Rating());
            sum += mWeight1[i][jj];
        }
        for (int jj = itemId * mNumPossibleRating; jj < itemId*5+5; ++jj) {
            sum += mWeight1[i][jj] * 0.2;
        }
        code[i] = Math::Sigmoid(sum);
    }
    Vector64F output(mNumPossibleRating);
    for (int i = itemId * mNumPossibleRating, ii = 0; ii < mNumPossibleRating; ++i, ++ii) {
        double sum = mBias2[i];
        for (int j = 0; j < mNumHiddenUnit; ++j) {
            sum += mWeight2[i][j] * code[j];
        }
        output[ii] = Math::Sigmoid(sum);
    }
    return std::move(output);
}

void CFAutoEncoder::Train(const CFAE::Parameter *param, const CFAE::TrainOption *trainOption,
        RatingMatrixAsUsers<> *data) {
    mTrainData = data;
    mNumPossibleRating = param->PossibleRatings();
    mNumInputUnit = mNumOutputUnit = data->NumItem() * mNumPossibleRating;
    mWeight1 = std::move(Matrix64F(mNumHiddenUnit, mNumInputUnit));
    mBias1 = std::move(Vector64F(mNumHiddenUnit));
    mWeight2 = std::move(Matrix64F(mNumInputUnit, mNumHiddenUnit));
    mBias2 = std::move(Vector64F(mNumInputUnit));
    CFAutoEncoderComputation *computationDelegate;
     if (!trainOption->Accelerate()) {
         computationDelegate = new CFAutoEncoderComputationST();
     } else {
         computationDelegate = new CFAutoEncoderComputationMT();
     }
     computationDelegate->Train(this, trainOption, data);
     delete computationDelegate;
}

//void CFAutoEncoder::Train(const CFAE::TrainOption *trainOption, RatingMatrixAsItems<> *data) {
//    Log::I("recsys", "CFAutoEncoder::Train()");
//    throw LonganNotSupportError();
//}

void CFAutoEncoder::Load(const std::string& filename) {
    BinaryInputStream bis(filename);
    bis >> mNumPossibleRating >> mNumInputUnit >> mNumHiddenUnit;
    mNumOutputUnit = mNumInputUnit;
    mWeight1 = std::move(Matrix64F(mNumHiddenUnit, mNumInputUnit));
    mBias1 = std::move(Vector64F(mNumHiddenUnit));
    mWeight2 = std::move(Matrix64F(mNumInputUnit, mNumHiddenUnit));
    mBias2 = std::move(Vector64F(mNumInputUnit));
    bis.Read(mWeight1.Data(), mWeight1.Size());
    bis.Read(mBias1.Data(), mBias1.Size());
    bis.Read(mWeight2.Data(), mWeight2.Size());
    bis.Read(mBias2.Data(), mBias2.Size());
    int numUser;
    bis >> numUser;
    mCodes.reserve(numUser);
    for (int uid = 0; uid < numUser; ++uid) {
        Vector64F code(mNumHiddenUnit);
        bis.Read(code.Data(), code.Size());
        mCodes.push_back(std::move(code));
    }
}

void CFAutoEncoder::Save(const std::string& filename) const {
    assert(mTrainData != nullptr);
    BinaryOutputStream bos(filename);
    bos << mNumPossibleRating << mNumInputUnit << mNumHiddenUnit;
    bos.Write(mWeight1.Data(), mWeight1.Size());
    bos.Write(mBias1.Data(), mBias1.Size());
    bos.Write(mWeight2.Data(), mWeight2.Size());
    bos.Write(mBias2.Data(), mBias2.Size());
    bos << mTrainData->NumUser();
    for (int uid = 0; uid < mTrainData->NumUser(); ++uid) {
        const Vector64F code = Encode(mTrainData->GetUserVector(uid));
        bos.Write(code.Data(), code.Size());
    }
}

Vector64F CFAutoEncoder::Encode(const UserVector<>& input) const {
    Vector64F code(mNumHiddenUnit);
    const ItemRating *data = input.Data();
    int size = input.Size();
    for (int i = 0; i < mNumHiddenUnit; ++i) {
        double sum = mBias1[i];
        for (int j = 0; j < size; ++j) {
            int jj = data[j].ItemId() * mNumPossibleRating
                    + static_cast<int>(data[j].Rating());
            sum += mWeight1[i][jj];
        }
        code[i] = Math::Sigmoid(sum);
    }
    return std::move(code);
}

Vector64F CFAutoEncoder::Decode(const Vector64F& code) const {
    Vector64F output(mNumOutputUnit);
    for (int i = 0; i < mNumOutputUnit; ++i) {
        double sum = mBias2[i];
        for (int j = 0; j < mNumHiddenUnit; ++j) {
            sum += mWeight2[i][j] * code[j];
        }
        output[i] = Math::Sigmoid(sum);
    }
    return std::move(output);
}

} //~ namespace longan
