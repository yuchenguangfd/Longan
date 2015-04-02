/*
 * netflix_prepare.h
 * Created on: Aug 5, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_PROJECT_NETFLIX_NETFLIX_PREPARE_H
#define RECSYS_PROJECT_NETFLIX_NETFLIX_PREPARE_H

#include "recsys/base/rating_record_with_time.h"
#include <json/json.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace longan {

struct Movie {
    int id;
    int year;
    std::string title;
};

struct UserItemPair {
    int userId;
    int itemId;
};

class UserItemPairHash {
public:
    int operator() (const UserItemPair& pair) const {
        return pair.userId + pair.itemId;
    }
};

class UserItemPairPred {
public:
    bool operator() (const UserItemPair& lhs, const UserItemPair& rhs) const {
        return (lhs.userId == rhs.userId) && (lhs.itemId == rhs.itemId);
    }
};

class NetflixPrepare {
public:
    NetflixPrepare(const std::string& inputDirpath, const std::string& configFilepath,
            const std::string& outputDirpath);
    void Prepare();
private:
    void LoadConfig();
    void ReadItemInfo();
    void ReadAllRating();
    void ReadRatingFile(const std::string& filename);
    int GetItemIdFromFileName(const std::string& filename);
    int GetTimestampFromDate(const std::string& date);
    void GenerateUserIdMapping();
    void GenerateItemIdMapping();
    void GenerateMovieData();
    void GenerateRatingData();
    void ReadTestRating();
    void SetTrainOrTestFlag();
    void GenerateTrainRatings();
    void GenerateTestRatings();
    void GenerateRatingDataBySplitAll();
    void GenerateRatingDataBySplitUser();
    void Cleanup();
private:
    const std::string mInputDirpath;
    const std::string mConfigFilepath;
    const std::string mOutputDirpath;
    Json::Value mConfig;
    int mNumUser;
    int mNumItem;
    int mNumRating;
    int mNumTestRatings;
    std::vector<Movie> mMovies;
    std::vector<RatingRecordWithTime*> mRatings;
    std::unordered_map<int, int> mUserIdMap;
    std::unordered_map<int, int> mItemIdMap;
    std::unordered_set<UserItemPair, UserItemPairHash, UserItemPairPred> mTestRatingSet;
    std::vector<bool> mIsTestRatingFlags;
};

} //~ namespace longan

#endif /* RECSYS_PROJECT_NETFLIX_NETFLIX_PREPARE_H */
