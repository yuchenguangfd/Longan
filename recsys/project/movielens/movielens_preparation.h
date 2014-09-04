/*
 * movielens_preparation.h
 * Created on: Aug 20, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_PROJECT_MOVIELENS_MOVIELENS_PREPARATION_H
#define RECSYS_PROJECT_MOVIELENS_MOVIELENS_PREPARATION_H

#include "recsys/util/rating_record_with_time.h"
#include <string>
#include <vector>
#include <map>

namespace longan {

struct Movie {
    int id;
    std::string title;
    int year;
};

class MovielensPreparation {
public:
    MovielensPreparation(const std::string& inputPath, const std::string& outputPath, double trainRatio);
    void PrepareDataset100K();
    void PrepareDataset1M();
    void PrepareDataset10M();
private:
    void PrepareDataset100K_ReadItemInfo();
    void PrepareDataset100K_ReadRatings();
    std::string PrepareDataset100K_ExtractTitle(const std::string& s);
    int PrepareDataset100K_ExtractYear(const std::string& s);

    void PrepareDataset1M_ReadItemInfo();
    void PrepareDataset1M_ReadRatings();
    std::string PrepareDataset1M_ExtractTitle(const std::string& s);
    int PrepareDataset1M_ExtractYear(const std::string& s);

    void PrepareDataset10M_ReadItemInfo();
    void PrepareDataset10M_ReadRatings();
    std::string PrepareDataset10M_ExtractTitle(const std::string& s);
    int PrepareDataset10M_ExtractYear(const std::string& s);

    void GenerateUserIdMapping();
    void GenerateItemIdMapping();
    void GenerateMovieData();
    void GenerateRatingData();
    void FreeRatings();
private:
    std::string mInputPath;
    std::string mOutputPath;
    double mTrainRatio;
    int mNumUser;
    int mNumItem;
    int mNumRating;
    std::vector<Movie> mMovies;
    std::vector<RatingRecordWithTime*> mRatings;
    std::map<int, int> mUserIdMap;
    std::map<int, int> mItemIdMap;
};

} //~ namespace longan

#endif /* RECSYS_PROJECT_MOVIELENS_MOVIELENS_PREPARATION_H */
