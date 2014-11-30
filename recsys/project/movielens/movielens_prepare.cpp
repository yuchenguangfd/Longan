/*
 * movielens_preparation.cpp
 * Created on: Aug 20, 2014
 * Author: chenguangyu
 */

#include "movielens_prepare.h"
#include "common/logging/logging.h"
#include "common/system/file_util.h"
#include "common/util/string_helper.h"
#include "common/util/array_helper.h"
#include "common/lang/integer.h"
#include "common/error.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>
#include <cassert>

namespace longan {

MovielensPrepare::MovielensPrepare(const std::string& inputDirpath, const std::string& configFilepath,
        const std::string& outputDirpath) :
    mInputDirpath(inputDirpath),
    mConfigFilepath(configFilepath),
    mOutputDirpath(outputDirpath),
    mNumUser(0), mNumItem(0), mNumRating(0) { }

void MovielensPrepare::Prepare() {
    LoadConfig();
    if (mConfig["datasetType"].asString() == "100k") {
        PrepareDataset100K();
    } else if (mConfig["datasetType"].asString() == "1m") {
        PrepareDataset1M();
    } else if (mConfig["datasetType"].asString() == "10m") {
        PrepareDataset10M();
    } else {
        throw LonganConfigError();
    }
}

void MovielensPrepare::LoadConfig() {
    Log::I("recsys", "MovielensPrepare::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    std::string content = FileUtil::LoadFileContent(mConfigFilepath);
    Json::Reader reader;
    if (!reader.parse(content, mConfig)) {
        throw LonganFileFormatError();
    }
}

void MovielensPrepare::PrepareDataset100K() {
    Log::I("recsys", "MovielensPrepare::PrepareDataset100K()");
    PrepareDataset100K_ReadItemInfo();
    PrepareDataset100K_ReadRatings();
    GenerateUserIdMapping();
    GenerateItemIdMapping();
    GenerateMovieData();
    GenerateRatingData();
    Cleanup();
}

void MovielensPrepare::PrepareDataset100K_ReadItemInfo() {
    Log::I("recsys", "MovielensPrepare::PrepareDataset100K_ReadItemInfo()");
    using namespace std;
    ifstream fin;
    string itemInfoFile = mInputDirpath + "/ml-100k/u.item";
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

std::string MovielensPrepare::PrepareDataset100K_ExtractTitle(const std::string& s) {
    using namespace std;
    if (s == "unknown") return s;
    auto begPos = 0;
    auto endPos = s.find('(');
    assert(endPos != string::npos);
    string title = s.substr(begPos, endPos - begPos);
    return title;
}

int MovielensPrepare::PrepareDataset100K_ExtractYear(const std::string& s) {
    using namespace std;
    if (s.empty()) return 9999;
    auto begPos = s.rfind('-') + 1;
    assert(begPos != string::npos);
    std::string yearStr = s.substr(begPos);
    return Integer::ParseInt(yearStr);
}

void MovielensPrepare::PrepareDataset100K_ReadRatings() {
    Log::I("recsys", "MovielensPrepare::PrepareDataset100K_ReadRatings()");
    using namespace std;
    ifstream fin;
    string ratingFile = mInputDirpath + "/ml-100k/u.data";
    fin.open(ratingFile.c_str());
    assert(!fin.fail());
    mRatings.clear();
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        int userId, itemId, rating, timestamp;
        iss >> userId >> itemId >> rating >> timestamp;
        RatingRecordWithTime *rr = new RatingRecordWithTime(userId, itemId, rating, timestamp);
        mRatings.push_back(rr);
    }
    mNumRating = mRatings.size();
    // sort by rating's timestamp
    sort(mRatings.begin(), mRatings.end(),
            [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs) {
        return lhs->Timestamp() < rhs->Timestamp();
    });
}

void MovielensPrepare::PrepareDataset1M() {
    Log::I("recsys", "MovielensPrepare::PrepareDataset1M()");
    PrepareDataset1M_ReadItemInfo();
    PrepareDataset1M_ReadRatings();
    GenerateUserIdMapping();
    GenerateItemIdMapping();
    GenerateMovieData();
    GenerateRatingData();
    Cleanup();
}

void MovielensPrepare::PrepareDataset1M_ReadItemInfo() {
    Log::I("recsys", "MovielensPrepare::PrepareDataset1M_ReadItemInfo()");
    using namespace std;
    ifstream fin;
    string itemInfoFile = mInputDirpath + "/ml-1m/movies.dat";
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

std::string MovielensPrepare::PrepareDataset1M_ExtractTitle(const std::string& s) {
    using namespace std;
    auto begPos = 0;
    auto endPos = s.rfind('(');
    assert(endPos != string::npos);
    string title = s.substr(begPos, endPos - begPos);
    return title;
}

int MovielensPrepare::PrepareDataset1M_ExtractYear(const std::string& s) {
    using namespace std;
    auto begPos = s.rfind('(');
    auto endPos = s.rfind(')');
    assert(begPos != string::npos && endPos != string::npos);
    ++begPos;
    string yearStr = s.substr(begPos, endPos - begPos);
    return Integer::ParseInt(yearStr);
}

void MovielensPrepare::PrepareDataset1M_ReadRatings(){
    Log::I("recsys", "MovielensPrepare::PrepareDataset1M_ReadRatings()");;
    using namespace std;
    ifstream fin;
    string ratingFile = mInputDirpath + "/ml-1m/ratings.dat";
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
        RatingRecordWithTime *rr = new RatingRecordWithTime(userId, itemId, rating, timestamp);
        mRatings.push_back(rr);
    }
    mNumRating = mRatings.size();
    // sort by rating's timestamp
    sort(mRatings.begin(), mRatings.end(),
            [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs) {
        return lhs->Timestamp() < rhs->Timestamp();
    });
}

void MovielensPrepare::PrepareDataset10M() {
    Log::I("recsys", "MovielensPrepare::PrepareDataset10M()");
    PrepareDataset10M_ReadItemInfo();
    PrepareDataset10M_ReadRatings();
    GenerateUserIdMapping();
    GenerateItemIdMapping();
    GenerateMovieData();
    GenerateRatingData();
    Cleanup();
}

void MovielensPrepare::PrepareDataset10M_ReadItemInfo() {
    Log::I("recsys", "MovielensPrepare::PrepareDataset10M_ReadItemInfo()");
    using namespace std;
    ifstream fin;
    string itemInfoFile = mInputDirpath + "/ml-10M100K/movies.dat";
    fin.open(itemInfoFile.c_str());
    assert(!fin.fail());
    mMovies.clear();
    string line;
    while (getline(fin, line)) {
        vector<string> lineFields = StringHelper::Split(line, "::", false);
        Movie movie;
        movie.id = Integer::ParseInt(lineFields[0]);
        movie.title = PrepareDataset10M_ExtractTitle(lineFields[1]);
        movie.year = PrepareDataset10M_ExtractYear(lineFields[1]);
        mMovies.push_back(movie);
    }
    // sort by movie's distribution year
    sort(mMovies.begin(), mMovies.end(),
            [](const Movie& lhs, const Movie& rhs){
        return lhs.year < rhs.year;
    });
}

std::string MovielensPrepare::PrepareDataset10M_ExtractTitle(
        const std::string& s) {
    return PrepareDataset1M_ExtractTitle(s);
}

int MovielensPrepare::PrepareDataset10M_ExtractYear(const std::string& s) {
    return PrepareDataset1M_ExtractYear(s);
}

void MovielensPrepare::PrepareDataset10M_ReadRatings() {
    Log::I("recsys", "MovielensPrepare::PrepareDataset10M_ReadRatings()");
    using namespace std;
    ifstream fin;
    string ratingFile = mInputDirpath + "/ml-10M100K/ratings.dat";
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

void MovielensPrepare::GenerateUserIdMapping() {
    Log::I("recsys", "MovielensPrepare::GenerateUserIdMapping()");
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

    string filename = mOutputDirpath + "/user_id_mapping.txt";
    ofstream fout(filename.c_str());
    assert(!fout.fail());
    fout << mNumUser << endl;
    for (auto it = mUserIdMap.begin(); it != mUserIdMap.end(); ++it) {
        fout << it->first << ',' << it->second << endl;
    }
}

void MovielensPrepare::GenerateItemIdMapping() {
    Log::I("recsys", "MovielensPrepare::GenerateItemIdMapping()");
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

    string filename = mOutputDirpath + "/item_id_mapping.txt";
    ofstream fout(filename.c_str());
    assert(!fout.fail());
    fout << mNumItem << endl;
    for (auto it = mItemIdMap.begin(); it != mItemIdMap.end(); ++it) {
       fout << it->first << "," << it->second << endl;
    }
}

void MovielensPrepare::GenerateMovieData() {
    Log::I("recsys", "MovielensPrepare::GenerateMovieData()");
    using namespace std;
    string filename = mOutputDirpath + "/movie.txt";
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

void MovielensPrepare::GenerateRatingData() {
    Log::I("recsys", "MovielensPrepare::GenerateRatingData()");
    using namespace std;
    ArrayHelper::RandomShuffle(&mRatings[0], mRatings.size());
    double trainRatio = mConfig["trainRatio"].asDouble();
    int splitPos = static_cast<int>(mNumRating * trainRatio);
    
    string filenameTrain = mOutputDirpath + "/rating_train.txt";
    Log::I("recsys", "writing train ratings to file = " + filenameTrain);
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

    string filenameTest = mOutputDirpath + "/rating_test.txt";
    Log::I("recsys", "writing test ratings to file: " + filenameTest);
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

void MovielensPrepare::Cleanup() {
    Log::I("recsys", "MovielensPrepare::Cleanup()");
    for (RatingRecordWithTime* pRating : mRatings) {
        delete pRating;
    }
}

} //~ namespace longan

