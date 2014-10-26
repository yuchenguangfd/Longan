/*
 * netflix_preparation.h
 * Created on: Aug 5, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_PROJECT_NETFLIX_NETFLIX_PREPARATION_H
#define RECSYS_PROJECT_NETFLIX_NETFLIX_PREPARATION_H

#include "recsys/base/rating_record_with_time.h"
#include <string>
#include <vector>
#include <map>
#include <set>
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

inline bool operator < (const UserItemPair& lhs, const UserItemPair& rhs) {
    if (lhs.userId != rhs.userId) {
        return lhs.userId < rhs.userId;
    }
    return lhs.itemId < rhs.itemId;
}


class NetflixPreparation {
public:
    NetflixPreparation(const std::string& inputPath, const std::string& outputPath);
    void PrepareDataset();
private:
    void ReadItemInfo();
    void ReadAllRating();
    void GenerateUserIdMapping();
    void GenerateItemIdMapping();
    void GenerateMovieData();
    void GenerateRatingData();
    void FreeRatings();
    void ReadRatingFile(const std::string& filename);
    void ReadTestRating();
    void SetTrainOrTestFlag();
    void GenerateTrainRatings();
    void GenerateTestRatings();
    int GetItemIdFromFileName(const std::string& filename);
    int GetTimestampFromDate(const std::string& date);
private:
    std::string mInputPath;
    std::string mOutputPath;
    int mNumUser;
    int mNumItem;
    int mNumRating;
    int mNumTestRatings;
    std::vector<Movie> mMovies;
    std::vector<RatingRecordWithTime*> mRatings;
    std::map<int, int> mUserIdMap;
    std::map<int, int> mItemIdMap;
    std::set<UserItemPair> mTestRatingSet;
    std::vector<bool> mIsTestRatingFlags;
};

} //~ namespace longan

#endif /* RECSYS_PROJECT_NETFLIX_NETFLIX_PREPARATION_H */
