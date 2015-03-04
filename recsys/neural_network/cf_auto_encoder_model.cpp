/*
 * cf_auto_encoder.cpp
 * Created on: Feb 3, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_model.h"
#include "common/common.h"

namespace longan {

namespace CFAE {

Model::Model(const Parameter *param, int numInputUnit, int numSample) :
    mParameter(param),
    mNumInputUnit(numInputUnit),
    mNumSample(numSample) {
    mEncodeWeights.resize(mParameter->NumLayer());
    mEncodeBiases.resize(mParameter->NumLayer());
    for (int layer = 0; layer < mParameter->NumLayer(); ++layer) {
        int rows = mParameter->NumLayerUnit(layer);
        int cols = (layer == 0) ? mNumInputUnit : mParameter->NumLayerUnit(layer-1);
        mEncodeWeights[layer] = std::move(Matrix64F(rows, cols));
        mEncodeBiases[layer] = std::move(Vector64F(rows));
    }
    mDecodeWeights.resize(mParameter->NumLayer());
    mDecodeBiases.resize(mParameter->NumLayer());
    for (int layer = 0; layer < mParameter->NumLayer(); ++layer) {
        int rows = (layer == 0) ? mNumInputUnit : mParameter->NumLayerUnit(layer-1);
        int cols = mParameter->NumLayerUnit(layer);
        mDecodeWeights[layer] = std::move(Matrix64F(rows, cols));
        mDecodeBiases[layer] = std::move(Vector64F(rows));
    }
    mCodes.resize(mNumSample);
    int codeLen = mParameter->NumLayerUnit(mParameter->NumLayer()-1);
    for (int i = 0; i < mNumSample; ++i) {
        mCodes[i] = std::move(Vector64F(codeLen));
    }
}

void Model::RandomInit() {
    double low = -0.05, high = +0.05;
    for (Matrix64F& weight : mEncodeWeights) {
        ArrayHelper::FillRandom(weight.Data(), weight.Size(), low, high);
    }
    for (Vector64F& bias : mEncodeBiases) {
        ArrayHelper::FillRandom(bias.Data(), bias.Size(), low, high);
    }
    for (Matrix64F& weight : mDecodeWeights) {
        ArrayHelper::FillRandom(weight.Data(), weight.Size(), low, high);
    }
    for (Vector64F& bias : mDecodeBiases) {
        ArrayHelper::FillRandom(bias.Data(), bias.Size(), low, high);
    }
}

void Model::Save(const std::string& filename) const {
    BinaryOutputStream bos(filename);
    bos << mNumInputUnit << mNumSample;
    bos << mParameter->NumLayer();
    for (int i = 0; i < mParameter->NumLayer(); ++i) {
        bos << mParameter->NumLayerUnit(i);
    }
    for (int i = 0; i < mParameter->NumLayer(); ++i) {
        bos.Write(mEncodeWeights[i].Data(), mEncodeWeights[i].Size());
        bos.Write(mEncodeBiases[i].Data(), mEncodeBiases[i].Size());
    }
    for (int i = 0; i < mParameter->NumLayer(); ++i) {
        bos.Write(mDecodeWeights[i].Data(), mDecodeWeights[i].Size());
        bos.Write(mDecodeBiases[i].Data(), mDecodeBiases[i].Size());
    }
    for (int i = 0; i < mNumSample; ++i) {
        bos.Write(mCodes[i].Data(), mCodes[i].Size());
    }
}

void Model::Load(const std::string& filename) {
    BinaryInputStream bis(filename);
    int numInputUnit, numSample;
    bis >> numInputUnit >> numSample;
    assert(numInputUnit == mNumInputUnit);
    assert(numSample == mNumSample);
    int numLayer;
    bis >> numLayer;
    for (int i = 0; i < mParameter->NumLayer(); ++i) {
        int numLayerUnit;
        bis >> numLayerUnit;
        assert(numLayerUnit == mParameter->NumLayerUnit(i));
    }
    for (int i = 0; i < mParameter->NumLayer(); ++i) {
        bis.Read(mEncodeWeights[i].Data(), mEncodeWeights[i].Size());
        bis.Read(mEncodeBiases[i].Data(), mEncodeBiases[i].Size());
    }
    for (int i = 0; i < mParameter->NumLayer(); ++i) {
        bis.Read(mDecodeWeights[i].Data(), mDecodeWeights[i].Size());
        bis.Read(mDecodeBiases[i].Data(), mDecodeBiases[i].Size());
    }
    for (int i = 0; i < mNumSample; ++i) {
        bis.Read(mCodes[i].Data(), mCodes[i].Size());
    }
}

} //~ namespace CFAE

//Vector64F CFAutoEncoder::Reconstruct(const UserVector<>& input) const {
//    return std::move(Decode(Encode(input)));
//}
//
//Vector64F CFAutoEncoder::Reconstruct(int userId) const {
//    assert(0 <= userId && userId < mCodes.size());
//    return std::move(Decode(mCodes[userId]));
//}
//
//Vector64F CFAutoEncoder::Reconstruct(int userId, int itemId) const {
//    assert(0 <= userId && userId < mCodes.size());
//    const Vector64F& code = mCodes[userId];
//    Vector64F output(mNumPossibleRating);
//    for (int i = itemId * mNumPossibleRating, ii = 0; ii < mNumPossibleRating; ++i, ++ii) {
//        double sum = mBias2[i];
//        for (int j = 0; j < mNumHiddenUnit; ++j) {
//            sum += mWeight2[i][j] * code[j];
//        }
//        output[ii] = Math::Sigmoid(sum);
//    }
//    return std::move(output);
//}
//
//Vector64F CFAutoEncoder::Reconstruct(const UserVector<>& input, int itemId) const {
//    Vector64F code(mNumHiddenUnit);
//    const ItemRating *data = input.Data();
//    int size = input.Size();
//    for (int i = 0; i < mNumHiddenUnit; ++i) {
//        double sum = mBias1[i];
//        for (int j = 0; j < size; ++j) {
//            int jj = data[j].ItemId() * mNumPossibleRating
//                    + static_cast<int>(data[j].Rating());
//            sum += mWeight1[i][jj];
//        }
//        for (int jj = itemId * mNumPossibleRating; jj < itemId*5+5; ++jj) {
//            sum += mWeight1[i][jj] * 0.2;
//        }
//        code[i] = Math::Sigmoid(sum);
//    }
//    Vector64F output(mNumPossibleRating);
//    for (int i = itemId * mNumPossibleRating, ii = 0; ii < mNumPossibleRating; ++i, ++ii) {
//        double sum = mBias2[i];
//        for (int j = 0; j < mNumHiddenUnit; ++j) {
//            sum += mWeight2[i][j] * code[j];
//        }
//        output[ii] = Math::Sigmoid(sum);
//    }
//    return std::move(output);
//}
//
//void CFAutoEncoder::Train(const CFAE::Parameter *param, const CFAE::TrainOption *trainOption,
//        RatingMatrixAsUsers<> *data) {
//    mTrainData = data;
//    mNumPossibleRating = param->PossibleRatings();
//    mNumInputUnit = mNumOutputUnit = data->NumItem() * mNumPossibleRating;
//
//    CFAutoEncoderComputation *computationDelegate;
//     if (!trainOption->Accelerate()) {
//         computationDelegate = new CFAutoEncoderComputationST();
//     } else {
//         computationDelegate = new CFAutoEncoderComputationMT();
//     }
//     computationDelegate->Train(this, trainOption, data);
//     delete computationDelegate;
//}
//
//Vector64F CFAutoEncoder::Encode(const UserVector<>& input) const {
//    Vector64F code(mNumHiddenUnit);
//    const ItemRating *data = input.Data();
//    int size = input.Size();
//    for (int i = 0; i < mNumHiddenUnit; ++i) {
//        double sum = mBias1[i];
//        for (int j = 0; j < size; ++j) {
//            int jj = data[j].ItemId() * mNumPossibleRating
//                    + static_cast<int>(data[j].Rating());
//            sum += mWeight1[i][jj];
//        }
//        code[i] = Math::Sigmoid(sum);
//    }
//    return std::move(code);
//}
//
//Vector64F CFAutoEncoder::Decode(const Vector64F& code) const {
//    Vector64F output(mNumOutputUnit);
//    for (int i = 0; i < mNumOutputUnit; ++i) {
//        double sum = mBias2[i];
//        for (int j = 0; j < mNumHiddenUnit; ++j) {
//            sum += mWeight2[i][j] * code[j];
//        }
//        output[i] = Math::Sigmoid(sum);
//    }
//    return std::move(output);
//}

} //~ namespace longan
