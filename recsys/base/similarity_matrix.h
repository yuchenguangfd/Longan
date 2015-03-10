/*
 * similarity_matrix.h
 * Created on: Mar 8, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_SIMILARITY_MATRIX_H
#define RECSYS_BASE_SIMILARITY_MATRIX_H

#include <vector>

namespace longan {

class SimilarityMatrix {
public:
    SimilarityMatrix(int size);
    void Put(int i, int j, float sim) {
        if (i < j) {
            mMat[j][i] = sim;
        } else {
            mMat[i][j] = sim;
        }
    }
    float Get(int i, int j) const {
        if (i < j) {
            return mMat[j][i];
        } else {
            return mMat[i][j];
        }
    }
    void DirectPut(int i, int j, float sim) {
        mMat[i][j] = sim; // j < i
    }
    float DirectGet(int i, int j) const {
        return mMat[i][j]; // j < i
    }
protected:
    int mSize;
    std::vector<std::vector<float>> mMat;
};

} //~ namespace longan

#endif /* RECSYS_BASE_SIMILARITY_MATRIX_H */
