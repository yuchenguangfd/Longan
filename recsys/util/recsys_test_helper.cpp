/*
 * recsys_test_helper.cpp
 * Created on: Nov 10, 2014
 * Author: chenguangyu
 */

#include "recsys_test_helper.h"
#include "common/util/array_helper.h"
#include "common/util/random.h"

namespace longan {

RatingList RecsysTestHelper::CreateRandomRatingList(int numUser, int numItem, int numRating) {
    int **mat;
    ArrayHelper::CreateArray2D(&mat, numUser, numItem);
    ArrayHelper::InitArray2D(mat, numUser, numItem, -1);
    for (int i = 0; i < numRating; ++i) {
        while (true) {
            int r = Random::Instance().Uniform(0, numUser);
            int c = Random::Instance().Uniform(0, numItem);
            if (mat[r][c] == -1) {
                mat[r][c] = Random::Instance().Uniform(1, 6);
                break;
            }
        }
    }
    RatingList rlist(numUser, numItem, numRating);
    for (int i = 0; i < numUser; ++i) {
        for (int j = 0; j < numItem; ++j) {
            if (mat[i][j] != -1) {
                rlist.Add(RatingRecord(i, j, mat[i][j]));
            }
        }
    }
    return std::move(rlist);
}

RatingMatItems RecsysTestHelper::CreateRandomRatingMatItems(int numUser, int numItem, int numRating) {
    RatingList rlist = CreateRandomRatingList(numUser, numItem, numRating);
    RatingMatItems rmat;
    rmat.Init(rlist);
    return std::move(rmat);
}

RatingMatrixAsUsers<> RecsysTestHelper::CreateRandomRatingMatrixAsUsers(int numUser, int numItem, int numRating) {
    RatingList rlist = CreateRandomRatingList(numUser, numItem, numRating);
    RatingMatrixAsUsers<> rmat;
    rmat.Init(rlist);
    return std::move(rmat);
}

} //~ namespace longan
