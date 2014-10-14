/*
 * rating_list_loader.h
 * Created on: Oct 14, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_RATING_LIST_LOADER_H
#define RECSYS_BASE_RATING_LIST_LOADER_H

#include "rating_list.h"

namespace longan {

class RatingListLoader {
public:
    static RatingList Load(const std::string& ratingFilename);
};

} //~ namespace longan

#endif /* RECSYS_BASE_RATING_LIST_LOADER_H */
