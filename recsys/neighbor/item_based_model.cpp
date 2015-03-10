/*
 * item_based_model.cpp
 * Created on: Nov 7, 2014
 * Author: chenguangyu
 */

#include "item_based_model.h"
#include "common/lang/binary_input_stream.h"
#include "common/lang/binary_output_stream.h"

namespace longan {

namespace ItemBased {

Model::Model(const Parameter *param, int numItem) :
    SimilarityMatrix(numItem),
    mParameter(param) { }

void Model::Save(const std::string& filename) {
    BinaryOutputStream bos(filename);
    bos << mSize;
    for (int i = 0; i < mSize; ++i) {
        bos.Write(mMat[i].data(), mMat[i].size());
    }
}

void Model::Load(const std::string& filename) {
    BinaryInputStream bis(filename);
    bis >> mSize;
    mMat.resize(mSize);
    for (int i = 0; i < mSize; ++i) {
        mMat[i].resize(i+1);
        bis.Read(mMat[i].data(), mMat[i].size());
    }
}

} //~ namespace ItemBased

} //~ namespace longan
