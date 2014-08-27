/*
 * netflix_preparation.h
 * Created on: Aug 5, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_PROJECT_NETFLIX_NETFLIX_PREPARATION_H
#define RECSYS_PROJECT_NETFLIX_NETFLIX_PREPARATION_H

#include "recsys/util/rating_record_with_time.h"
#include <string>
#include <vector>
#include <set>
#include <map>

namespace longan {

struct Movie {
    std::string id;
    std::string title;
    int year;
};

struct UserItemPair {
    int userId;
    int itemId;
};

inline bool operator < (const UserItemPair& lhs, const UserItemPair& rhs) {
    if (lhs.userId != rhs.userId) {
        return lhs.userId < rhs.userId;
    }
    return lhs.itemId < rhs.itemId;
}


class NetflixPreparation {
public:
    NetflixPreparation(const std::string& inputPath, const std::string& outputPath);
    void PrepareMovieData();
    void PrepareRatingData();
private:
    void LoadAllRatings();
    void LoadRatingsFromFile(const std::string& filename);
    void SortAllRatingsByTime();
    void GenerateUserIdMapping();
    void GenerateTrainAndTestRatings();
    void FreeAllRatings();
    void LoadItemIdMapping();
    void LoadTestRatingRecords();
    void SetTrainOrTestFlag();
    void GenerateTrainRatings();
    void GenerateTestRatings();
    int GetItemIdFromFileName(const std::string& filename);
    int GetTimestampFromDate(const std::string& date);
private:
    std::string mInputPath;
    std::string mOutputPath;
    std::vector<RatingRecordWithTime*> mAllRatings;
    std::map<int, int> mUserIdMap;
    std::map<int, int> mItemIdMap;
    std::set<UserItemPair> mTestRatingSet;
    std::vector<bool> mAllRatingsIsTestFlag;
    int mNumTestRatings;
};

} //~ namespace longan

#endif /* RECSYS_PROJECT_NETFLIX_NETFLIX_PREPARATION_H */
