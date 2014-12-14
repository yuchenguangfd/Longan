/*
 * svd_model.cpp
 * Created on: Dec 13, 2014
 * Author: chenguangyu
 */

#include "svd_model.h"
#include "common/lang/binary_input_stream.h"

namespace longan {

SVDModelPredict::SVDModelPredict() :
    mNumUser(0),
    mNumItem(0),
    mAvg(0),
    P(nullptr),
    Q(nullptr) { }

SVDModelPredict::~SVDModelPredict() {
    delete P;
    delete Q;
}

void SVDModelPredict::Load(const std::string& filename) {
    BinaryInputStream bis(filename);
    bis >> mParam.dim
        >> mParam.lp >> mParam.lq >> mParam.lub
        >> mParam.lib >> mParam.gamma;
    bis >> mNumUser >> mNumItem >> mAvg;
    P = new float[mNumUser * mParam.dim];
    bis.Read(P, mNumUser * mParam.dim);
    Q = new float[mNumItem * mParam.dim];
    bis.Read(Q, mNumItem * mParam.dim);
    if(mParam.lub >= 0) {
        UB.resize(mNumUser);
        bis.Read(UB.data(), mNumUser);
    }
    if(mParam.lib >= 0) {
        IB.resize(mNumItem);
        bis.Read(IB.data(), mNumItem);
    }
}

} //~ namespace longan
