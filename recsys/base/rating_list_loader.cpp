/*
 * rating_list_loader.cpp
 * Created on: Oct 14, 2014
 * Author: chenguangyu
 */

#include "rating_list_loader.h"
#include <cstdio>
#include <cassert>

namespace longan {

RatingList RatingListLoader::Load(const std::string& ratingFilename) {
    int rtn;
    FILE *fp = fopen(ratingFilename.c_str(), "r");
    assert(fp != nullptr);

    int numRating, numUser, numItem;
    rtn = fscanf(fp, "%d,%d,%d", &numRating, &numUser, &numItem);
    assert(rtn == 3);

    RatingList ratingList(numUser, numItem, numRating);
    for (int i = 0; i < numRating; ++i) {
        int userId, itemId, time;
        float rating;
        rtn = fscanf(fp, "%d,%d,%f,%d", &userId, &itemId, &rating, &time);
        assert(rtn == 4);
        ratingList.Add(RatingRecord(userId, itemId, rating));
    }
    fclose(fp);
    return std::move(ratingList);
}

} //~ namespace longan
