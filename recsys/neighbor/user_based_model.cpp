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

ModelTrain::ModelTrain(const Parameter *param, int numUser) :
    mParameter(param),
    mNumUser(numUser) {
    mSimMat.resize(mNumUser);
    for (int uid = 0; uid < mNumUser; ++uid) {
        mSimMat[uid].resize(uid+1);
    }
}

void ModelTrain::Save(const std::string& filename) {
    BinaryOutputStream bos(filename);
    bos << mNumUser;
    for (int uid = 0; uid < mNumUser; ++uid) {
        bos.Write(mSimMat[uid].data(), mSimMat[uid].size());
    }
}

ModelPredict::ModelPredict() :
    mNumUser(0) { }

void ModelPredict::Load(const std::string& filename) {
    BinaryInputStream bis(filename);
    bis >> mNumUser;
    mSimMat.resize(mNumUser);
    for (int uid = 0; uid < mNumUser; ++uid) {
        mSimMat[uid].resize(uid+1);
        bis.Read(mSimMat[uid].data(), mSimMat[uid].size());
    }
}

} //~ namespace UserBased

} //~ namespace longan
