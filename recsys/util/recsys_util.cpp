/*
 * recsys_util.cpp
 * Created on: Nov 10, 2014
 * Author: chenguangyu
 */

#include "recsys_util.h"
#include "common/common.h"

namespace longan {

namespace RecsysUtil {

RatingList RandomRatingList(int numUser, int numItem, int numRating,
        bool isBinaryRating, float minRating, float maxRating) {
    std::set<std::pair<int, int>> existPositions;
    Random& rand = Random::Instance();
    for (int i = 0; i < numRating; ++i) {
        while (true) {
            int userId = rand.Uniform(0, numUser);
            int itemId = rand.Uniform(0, numItem);
            std::pair<int, int> pos(userId, itemId);
            if (existPositions.find(pos) == existPositions.end()) {
                existPositions.insert(pos);
                break;
            }
        }
    }
    RatingList rlist(numUser, numItem, numRating);
    for (auto iter = existPositions.begin(); iter != existPositions.end(); ++iter) {
        int userId = iter->first;
        int itemId = iter->second;
        float rating;
        if (isBinaryRating) {
            rating = (float)(rand.NextInt() % 2);
        } else {
            rating = rand.Uniform(minRating, maxRating);
        }
        rlist.Add(RatingRecord(userId, itemId, rating));
    }
    return std::move(rlist);
}

RatingMatItems RandomRatingMatItems(int numUser, int numItem, int numRating,
        bool isBinaryRating, float minRating, float maxRating) {
    RatingList rlist = RandomRatingList(numUser, numItem, numRating, isBinaryRating, minRating, maxRating);
    RatingMatItems rmat;
    rmat.Init(rlist);
    return std::move(rmat);
}

RatingMatUsers RandomRatingMatUsers(int numUser, int numItem, int numRating,
        bool isBinaryRating, float minRating, float maxRating) {
    RatingList rlist = RandomRatingList(numUser, numItem, numRating, isBinaryRating, minRating, maxRating);
    RatingMatUsers rmat;
    rmat.Init(rlist);
    return std::move(rmat);
}

} //~ namespace RecsysUtil

} //~ namespace longan
