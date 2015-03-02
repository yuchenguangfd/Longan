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

ModelTrain::ModelTrain(const Parameter *param, int numItem) :
    mParameter(param),
    mNumItem(numItem) {
    mSimMat.resize(mNumItem);
    for (int iid = 0; iid < mNumItem; ++iid) {
        mSimMat[iid].resize(iid+1);
    }
}

void ModelTrain::Save(const std::string& filename) {
    BinaryOutputStream bos(filename);
    bos << mNumItem;
    for (int iid = 0; iid < mNumItem; ++iid) {
        bos.Write(mSimMat[iid].data(), mSimMat[iid].size());
    }
}

ModelPredict::ModelPredict() :
    mNumItem(0) { }

void ModelPredict::Load(const std::string& filename) {
    BinaryInputStream bis(filename);
    bis >> mNumItem;
    mSimMat.resize(mNumItem);
    for (int iid = 0; iid < mNumItem; ++iid) {
        mSimMat[iid].resize(iid+1);
        bis.Read(mSimMat[iid].data(), mSimMat[iid].size());
    }
}

} //~ namespace ItemBased

} //~ namespace longan
