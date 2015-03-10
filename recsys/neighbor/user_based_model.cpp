/*
 * user_based_model.cpp
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#include "user_based_model.h"
#include "common/lang/binary_input_stream.h"
#include "common/lang/binary_output_stream.h"

namespace longan {

namespace UserBased {

Model::Model(const Parameter *param, int numUser) :
    SimilarityMatrix(numUser),
    mParameter(param) { }

void Model::Save(const std::string& filename) {
    BinaryOutputStream bos(filename);
    bos << mSize;
    for (int uid = 0; uid < mSize; ++uid) {
        bos.Write(mMat[uid].data(), mMat[uid].size());
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

} //~ namespace UserBased

} //~ namespace longan
