/*
 * movielens_prepare.h
 * Created on: Aug 20, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_PROJECT_MOVIELENS_MOVIELENS_PREPARE_H
#define RECSYS_PROJECT_MOVIELENS_MOVIELENS_PREPARE_H

#include "recsys/base/rating_record_with_time.h"
#include <json/json.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace longan {

struct Movie {
    int id;
    std::string title;
    int year;
};

class MovielensPrepare {
public:
    MovielensPrepare(const std::string& inputDirpath, const std::string& configFilepath, const std::string& outputDirpath);
    void Prepare();
private:
    void LoadConfig();
    void PrepareDataset100K();
    void PrepareDataset100K_ReadItemInfo();
    void PrepareDataset100K_ReadRatings();
    std::string PrepareDataset100K_ExtractTitle(const std::string& s);
    int PrepareDataset100K_ExtractYear(const std::string& s);

    void PrepareDataset1M();
    void PrepareDataset1M_ReadItemInfo();
    void PrepareDataset1M_ReadRatings();
    std::string PrepareDataset1M_ExtractTitle(const std::string& s);
    int PrepareDataset1M_ExtractYear(const std::string& s);

    void PrepareDataset10M();
    void PrepareDataset10M_ReadItemInfo();
    void PrepareDataset10M_ReadRatings();
    std::string PrepareDataset10M_ExtractTitle(const std::string& s);
    int PrepareDataset10M_ExtractYear(const std::string& s);

    void GenerateUserIdMapping();
    void GenerateItemIdMapping();
    void GenerateMovieData();
    void GenerateRatingData();
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
    std::vector<Movie> mMovies;
    std::vector<RatingRecordWithTime*> mRatings;
    std::unordered_map<int, int> mUserIdMap;
    std::unordered_map<int, int> mItemIdMap;
};

} //~ namespace longan

#endif /* RECSYS_PROJECT_MOVIELENS_MOVIELENS_PREPARE_H */
