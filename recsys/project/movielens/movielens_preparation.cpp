/*
 * movielens_preparation.cpp
 * Created on: Aug 20, 2014
 * Author: chenguangyu
 */

#include "movielens_preparation.h"
#include "common/util/string_helper.h"
#include "common/util/array_helper.h"
#include "common/lang/integer.h"
#include "common/logging/logging_helper.h"
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>
#include <cassert>

namespace longan {

MovielensPreparation::MovielensPreparation(const std::string& inputPath, const std::string& outputPath, double trainRatio) :
    mInputPath(inputPath), mOutputPath(outputPath), mTrainRatio(trainRatio),
    mNumUser(0), mNumItem(0), mNumRating(0) { }

void MovielensPreparation::PrepareDataset100K() {
    LOG_FUNC;
    PrepareDataset100K_ReadItemInfo();
    PrepareDataset100K_ReadRatings();
    GenerateUserIdMapping();
    GenerateItemIdMapping();
    GenerateMovieData();
    GenerateRatingData();
    FreeRatings();
}

void MovielensPreparation::PrepareDataset100K_ReadItemInfo() {
    LOG_FUNC;
    using namespace std;
    ifstream fin;
    string itemInfoFile = mInputPath + "/ml-100k/u.item";
    fin.open(itemInfoFile.c_str());
    assert(!fin.fail());
    mMovies.clear();
    string line;
    while (getline(fin, line)) {
        vector<string> lineFields = StringHelper::Split(line, "|", false);
        Movie movie;
        movie.id = Integer::ParseInt(lineFields[0]);
        movie.title = PrepareDataset100K_ExtractTitle(lineFields[1]);
        movie.year = PrepareDataset100K_ExtractYear(lineFields[2]);
        mMovies.push_back(movie);
    }
    // sort by movie's distribution year
    sort(mMovies.begin(), mMovies.end(),
            [](const Movie& lhs, const Movie& rhs){
        return lhs.year < rhs.year;
    });
}

std::string MovielensPreparation::PrepareDataset100K_ExtractTitle(const std::string& s) {
    using namespace std;
    if (s == "unknown") return s;
    auto begPos = 0;
    auto endPos = s.find('(');
    assert(endPos != string::npos);
    string title = s.substr(begPos, endPos - begPos);
    return title;
}

int MovielensPreparation::PrepareDataset100K_ExtractYear(const std::string& s) {
    using namespace std;
    if (s.empty()) return 9999;
    auto begPos = s.rfind('-') + 1;
    assert(begPos != string::npos);
    std::string yearStr = s.substr(begPos);
    return Integer::ParseInt(yearStr);
}

void MovielensPreparation::PrepareDataset100K_ReadRatings() {
    LOG_FUNC;
    using namespace std;
    ifstream fin;
    string ratingFile = mInputPath + "/ml-100k/u.data";
    fin.open(ratingFile.c_str());
    assert(!fin.fail());
    mRatings.clear();
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        int userId, itemId, rating, timestamp;
        iss >> userId >> itemId >> rating >> timestamp;
        RatingRecordWithTime *pRating = new RatingRecordWithTime(userId, itemId, rating, timestamp);
        mRatings.push_back(pRating);
    }
    mNumRating = mRatings.size();
    // sort by rating's timestamp
    sort(mRatings.begin(), mRatings.end(),
            [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs) {
        return lhs->Timestamp() < rhs->Timestamp();
    });
}

void MovielensPreparation::PrepareDataset1M() {
    LOG_FUNC;
    PrepareDataset1M_ReadItemInfo();
    PrepareDataset1M_ReadRatings();
    GenerateUserIdMapping();
    GenerateItemIdMapping();
    GenerateMovieData();
    GenerateRatingData();
    FreeRatings();
}

void MovielensPreparation::PrepareDataset1M_ReadItemInfo() {
    LOG_FUNC;
    using namespace std;
    ifstream fin;
    string itemInfoFile = mInputPath + "/ml-1m/movies.dat";
    fin.open(itemInfoFile.c_str());
    assert(!fin.fail());
    mMovies.clear();
    string line;
    while (getline(fin, line)) {
        vector<string> lineFields = StringHelper::Split(line, "::", false);
        Movie movie;
        movie.id = Integer::ParseInt(lineFields[0]);
        movie.title = PrepareDataset1M_ExtractTitle(lineFields[1]);
        movie.year = PrepareDataset1M_ExtractYear(lineFields[1]);
        mMovies.push_back(movie);
    }
    // sort by movie's distribution year
    sort(mMovies.begin(), mMovies.end(),
            [](const Movie& lhs, const Movie& rhs){
        return lhs.year < rhs.year;
    });
}

std::string MovielensPreparation::PrepareDataset1M_ExtractTitle(const std::string& s) {
    using namespace std;
    auto begPos = 0;
    auto endPos = s.rfind('(');
    assert(endPos != string::npos);
    string title = s.substr(begPos, endPos - begPos);
    return title;
}

int MovielensPreparation::PrepareDataset1M_ExtractYear(const std::string& s) {
    using namespace std;
    auto begPos = s.rfind('(');
    auto endPos = s.rfind(')');
    assert(begPos != string::npos && endPos != string::npos);
    ++begPos;
    string yearStr = s.substr(begPos, endPos - begPos);
    return Integer::ParseInt(yearStr);
}

void MovielensPreparation::PrepareDataset1M_ReadRatings(){
    LOG_FUNC;
    using namespace std;
    ifstream fin;
    string ratingFile = mInputPath + "/ml-1m/ratings.dat";
    fin.open(ratingFile.c_str());
    assert(!fin.fail());
    mRatings.clear();
    string line;
    while (getline(fin, line)) {
        vector<string> lineFields = StringHelper::Split(line, "::", false);
        assert(lineFields.size() == 4);
        int userId = Integer::ParseInt(lineFields[0]);
        int itemId = Integer::ParseInt(lineFields[1]);
        int rating = Integer::ParseInt(lineFields[2]);
        int timestamp = Integer::ParseInt(lineFields[3]);
        RatingRecordWithTime *pRating = new RatingRecordWithTime(userId, itemId, rating, timestamp);
        mRatings.push_back(pRating);
    }
    mNumRating = mRatings.size();
    // sort by rating's timestamp
    sort(mRatings.begin(), mRatings.end(),
            [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs) {
        return lhs->Timestamp() < rhs->Timestamp();
    });
}

void MovielensPreparation::PrepareDataset10M() {
    LOG_FUNC;
    PrepareDataset10M_ReadItemInfo();
    PrepareDataset10M_ReadRatings();
    GenerateUserIdMapping();
    GenerateItemIdMapping();
    GenerateMovieData();
    GenerateRatingData();
    FreeRatings();
}

void MovielensPreparation::PrepareDataset10M_ReadItemInfo() {
    LOG_FUNC;
    using namespace std;
    ifstream fin;
    string itemInfoFile = mInputPath + "/ml-10M100K/movies.dat";
    fin.open(itemInfoFile.c_str());
    assert(!fin.fail());
    mMovies.clear();
    string line;
    while (getline(fin, line)) {
        vector<string> lineFields = StringHelper::Split(line, "::", false);
        Movie movie;
        movie.id = Integer::ParseInt(lineFields[0]);
        movie.title = PrepareDataset1M_ExtractTitle(lineFields[1]);
        movie.year = PrepareDataset1M_ExtractYear(lineFields[1]);
        mMovies.push_back(movie);
    }
    // sort by movie's distribution year
    sort(mMovies.begin(), mMovies.end(),
            [](const Movie& lhs, const Movie& rhs){
        return lhs.year < rhs.year;
    });
}

void MovielensPreparation::PrepareDataset10M_ReadRatings() {
    LOG_FUNC;
    using namespace std;
    ifstream fin;
    string ratingFile = mInputPath + "/ml-10M100K/ratings.dat";
    fin.open(ratingFile.c_str());
    assert(!fin.fail());
    mRatings.clear();
    string line;
    while (getline(fin, line)) {
        vector<string> lineFields = StringHelper::Split(line, "::", false);
        assert(lineFields.size() == 4);
        int userId = Integer::ParseInt(lineFields[0]);
        int itemId = Integer::ParseInt(lineFields[1]);
        int rating = Integer::ParseInt(lineFields[2]);
        int timestamp = Integer::ParseInt(lineFields[3]);
        RatingRecordWithTime *pRating = new RatingRecordWithTime(userId, itemId, rating, timestamp);
        mRatings.push_back(pRating);
    }
    mNumRating = mRatings.size();
    // sort by rating's timestamp
    sort(mRatings.begin(), mRatings.end(),
            [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs) {
        return lhs->Timestamp() < rhs->Timestamp();
    });
}

void MovielensPreparation::GenerateUserIdMapping() {
    LOG_FUNC;
    using namespace std;
    mUserIdMap.clear();
    int userCount = 0;
    for (int i = 0; i < mRatings.size(); ++i) {
        RatingRecordWithTime *pRating = mRatings[i];
        if (mUserIdMap.find(pRating->UserId()) == mUserIdMap.end()) {
            mUserIdMap[pRating->UserId()] = userCount;
            ++userCount;
        }
    }
    mNumUser = userCount;

    string filename = mOutputPath + "/user_id_mapping.txt";
    ofstream fout(filename.c_str());
    assert(!fout.fail());
    fout << mNumUser << endl;
    for (auto it = mUserIdMap.begin(); it != mUserIdMap.end(); ++it) {
        fout << it->first << ',' << it->second << endl;
    }
}

void MovielensPreparation::GenerateItemIdMapping() {
    LOG_FUNC;
    using namespace std;
    mItemIdMap.clear();
    int itemCount = 0;
    for (int i = 0; i < mRatings.size(); ++i) {
        RatingRecordWithTime *pRating = mRatings[i];
        if (mItemIdMap.find(pRating->ItemId()) == mItemIdMap.end()) {
            mItemIdMap[pRating->ItemId()] = itemCount;
            ++itemCount;
        }
    }
    mNumItem = itemCount;

    string filename = mOutputPath + "/item_id_mapping.txt";
    ofstream fout(filename.c_str());
    assert(!fout.fail());
    fout << mNumItem << endl;
    for (auto it = mItemIdMap.begin(); it != mItemIdMap.end(); ++it) {
       fout << it->first << "," << it->second << endl;
    }
}

void MovielensPreparation::GenerateMovieData() {
    LOG_FUNC;
    using namespace std;
    string filename = mOutputPath + "/movie.txt";
    ofstream fout(filename.c_str());
    assert(!fout.fail());

    int countValidMovie = 0;
    for (auto& movie : mMovies) {
        if (mItemIdMap.find(movie.id) != mItemIdMap.end()) {
            ++countValidMovie;
        }    
    }

    fout << countValidMovie << endl;
    for (auto& movie : mMovies) {
        if (mItemIdMap.find(movie.id) != mItemIdMap.end()) {
            fout << mItemIdMap[movie.id] << "," 
                 << movie.year << ","
                 << movie.title << endl;
        }
    }
}

void MovielensPreparation::GenerateRatingData() {
    LOG_FUNC;
    using namespace std;
    ArrayHelper::RandomShuffle(&mRatings[0], mRatings.size());
    int splitPos = static_cast<int>(mNumRating * mTrainRatio);
    
    string filenameTrain = mOutputPath + "/rating_train.txt";
    LOG(INFO) << "writing train ratings to file: " << filenameTrain;
    ofstream fout1(filenameTrain.c_str());
    assert(!fout1.fail());
    sort(&mRatings[0], &mRatings[splitPos],
        [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs) {
            return lhs->Timestamp() < rhs->Timestamp();
        });
    int numTrainRatings = splitPos;
    fout1 << numTrainRatings << ","
          << mNumUser << ","
          << mNumItem << endl;
    for (int i = 0; i < splitPos; ++i) {
        RatingRecordWithTime& rating = *mRatings[i];
        fout1 << mUserIdMap[rating.UserId()] << "," 
              << mItemIdMap[rating.ItemId()] << ","
              << rating.Rating() << "," 
              << rating.Timestamp() << endl;
    }

    string filenameTest = mOutputPath + "/rating_test.txt";
    LOG(INFO) << "writing test ratings to file: " << filenameTest;
    ofstream fout2(filenameTest.c_str());
    assert(!fout2.fail());
    sort(&mRatings[splitPos], &mRatings[0] + mRatings.size(),
        [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs) {
            return lhs->Timestamp() < rhs->Timestamp();
        });
    int numTestRatings = mRatings.size() - numTrainRatings;
    fout2 << numTestRatings << ","
          << mNumUser << ","
          << mNumItem << endl;
    for (int i = splitPos; i < mRatings.size(); ++i) {
        RatingRecordWithTime& rating = *mRatings[i];
        fout2 << mUserIdMap[rating.UserId()] << "," 
              << mItemIdMap[rating.ItemId()] << ","
              << rating.Rating() << "," 
              << rating.Timestamp() << endl;
    }
}

void MovielensPreparation::FreeRatings() {
    LOG_FUNC;
    for (RatingRecordWithTime* pRating : mRatings) {
        delete pRating;
    }
}

} //~ namespace longan
