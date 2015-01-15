/*
 * svd_model.cpp
 * Created on: Dec 13, 2014
 * Author: chenguangyu
 */

#include "svd_model.h"
#include "common/lang/binary_output_stream.h"
#include "common/lang/binary_input_stream.h"
#include "common/util/random.h"

namespace longan {

namespace SVD {

Model::Model(const Parameter& param) :
    mParameter(param),
    mNumUser(0),
    mNumItem(0),
    mRatingAverage(0.0f) { }

Model::~Model() { }

ModelTrain::ModelTrain(const Parameter& param, int numUser, int numItem, float ratingAvg) :
    Model(param) {
    mNumUser = numUser;
    mNumItem = numItem;
    mRatingAverage = ratingAvg;
}

void ModelTrain::RandomInit() {
    mUserFeatures.resize(mNumUser);
    for (int i = 0; i < mNumUser; ++i) {
        mUserFeatures[i] = Vector<float>::Rand(mParameter.Dim(), -0.1, +0.1);
    }
    mItemFeatures.resize(mNumItem);
    for (int i = 0; i < mNumItem; ++i) {
        mItemFeatures[i] = Vector<float>::Rand(mParameter.Dim(), -0.1, +0.1);
    }
    if (mParameter.LambdaUserBias() >= 0) {
        mUserBiases.assign(mNumUser, 0.0f);
    }
    if (mParameter.LambdaItemBias() >= 0) {
        mItemBiases.assign(mNumItem, 0.0f);
    }
}

void ModelTrain::Save(const std::string& filename) {
    BinaryOutputStream bos(filename);
    bos << mNumUser << mNumItem << mRatingAverage;
    assert(mUserFeatures.size() == mNumUser);
    for (int uid = 0; uid < mNumUser; ++uid) {
        const Vector<float>& vec = mUserFeatures[uid];
        assert(vec.Size() == mParameter.Dim());
        bos.Write(vec.Data(), vec.Size());
    }
    assert(mItemFeatures.size() == mNumItem);
    for (int iid = 0; iid < mNumItem; ++iid) {
        const Vector<float>& vec = mItemFeatures[iid];
        assert(vec.Size() == mParameter.Dim());
        bos.Write(vec.Data(), vec.Size());
    }
    if (mParameter.LambdaUserBias() >= 0) {
        assert(mUserBiases.size() == mNumUser);
        bos.Write(mUserBiases.data(), mUserBiases.size());
    }
    if (mParameter.LambdaItemBias() >= 0) {
        assert(mItemBiases.size() == mNumItem);
        bos.Write(mItemBiases.data(), mItemBiases.size());
    }
}

void ModelPredict::Load(const std::string& filename) {
    BinaryInputStream bis(filename);
    bis >> mNumUser >> mNumItem >> mRatingAverage;
    mUserFeatures.resize(mNumUser);
    for (int uid = 0; uid < mNumUser; ++uid) {
        Vector<float> vec(mParameter.Dim());
        bis.Read(vec.Data(), vec.Size());
        mUserFeatures[uid] = std::move(vec);
    }
    mItemFeatures.resize(mNumItem);
    for (int iid = 0; iid < mNumItem; ++iid) {
        Vector<float> vec(mParameter.Dim());
        bis.Read(vec.Data(), vec.Size());
        mItemFeatures[iid] = std::move(vec);
    }
    if(mParameter.LambdaUserBias() >= 0) {
        mUserBiases.resize(mNumUser);
        bis.Read(mUserBiases.data(), mUserBiases.size());
    }
    if(mParameter.LambdaItemBias() >= 0) {
        mItemBiases.resize(mNumItem);
        bis.Read(mItemBiases.data(), mItemBiases.size());
    }
}

} //~ namespace SVD

} //~ namespace longan
