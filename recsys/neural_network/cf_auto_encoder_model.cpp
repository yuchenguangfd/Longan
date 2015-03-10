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
    mCodeLength = mParameter->NumLayerUnit(mParameter->NumLayer()-1);
    for (int i = 0; i < mNumSample; ++i) {
        mCodes[i] = std::move(Vector64F(mCodeLength));
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

float Model::Reconstruct(int userId, int itemId) const {
    if (mParameter->CodeType() == Parameter::CodeType_User) {
        const Vector64F& code = mCodes[userId];
        float output = mDecodeBiases[0][itemId];
        for (int j = 0; j < mCodeLength; ++j) {
            output += mDecodeWeights[0][itemId][j] * code[j];
        }
        output = Math::Sigmoid(output);
        return output;
    } else if (mParameter->CodeType() == Parameter::CodeType_Item) {
        const Vector64F& code = mCodes[itemId];
        float output = mDecodeBiases[0][userId];
        for (int j = 0; j < mCodeLength; ++j) {
            output += mDecodeWeights[0][userId][j] * code[j];
        }
        output = Math::Sigmoid(output);
        return output;
    } else {
        return 0.0f;
    }
}

} //~ namespace CFAE

}//~ namespace longan

