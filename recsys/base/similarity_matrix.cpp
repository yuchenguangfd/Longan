/*
 * similarity_matrix.cpp
 * Created on: Mar 8, 2015
 * Author: chenguangyu
 */

#include "similarity_matrix.h"

namespace longan {

SimilarityMatrix::SimilarityMatrix(int size) :
    mSize(size) {
    mMat.resize(mSize);
    for (int i = 0; i < mSize; ++i) {
        mMat[i].resize(i+1);
    }
}

} //~ namespace longan
