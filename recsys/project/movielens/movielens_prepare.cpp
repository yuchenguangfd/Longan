/*
 * movielens_preparation.cpp
 * Created on: Aug 20, 2014
 * Author: chenguangyu
 */

#include "movielens_prepare.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "common/common.h"

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
    JsonConfigHelper::LoadFromFile(mConfigFilepath, mConfig);
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
    std::ifstream fin;
    std::string itemInfoFile = mInputDirpath + "/ml-100k/u.item";
    fin.open(itemInfoFile.c_str());
    assert(!fin.fail());
    mMovies.clear();
    std::string line;
    while (std::getline(fin, line)) {
        std::vector<std::string> lineFields = StringHelper::Split(line, "|", false);
        Movie movie;
        movie.id = Integer::ParseInt(lineFields[0]);
        movie.title = PrepareDataset100K_ExtractTitle(lineFields[1]);
        movie.year = PrepareDataset100K_ExtractYear(lineFields[2]);
        mMovies.push_back(movie);
    }
    // sort by movie's distribution year
    std::sort(mMovies.begin(), mMovies.end(),
            [](const Movie& lhs, const Movie& rhs){
        return lhs.year < rhs.year;
    });
}

std::string MovielensPrepare::PrepareDataset100K_ExtractTitle(const std::string& s) {
    if (s == "unknown") return s;
    auto begPos = 0;
    auto endPos = s.find('(');
    assert(endPos != std::string::npos);
    std::string title = s.substr(begPos, endPos - begPos);
    return title;
}

int MovielensPrepare::PrepareDataset100K_ExtractYear(const std::string& s) {
    if (s.empty()) return 9999;
    auto begPos = s.rfind('-') + 1;
    assert(begPos != std::string::npos);
    std::string yearStr = s.substr(begPos);
    return Integer::ParseInt(yearStr);
}

void MovielensPrepare::PrepareDataset100K_ReadRatings() {
    Log::I("recsys", "MovielensPrepare::PrepareDataset100K_ReadRatings()");
    std::ifstream fin;
    std::string ratingFile = mInputDirpath + "/ml-100k/u.data";
    fin.open(ratingFile.c_str());
    assert(!fin.fail());
    mRatings.clear();
    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        int userId, itemId, rating, timestamp;
        iss >> userId >> itemId >> rating >> timestamp;
        assert(1 <= rating && rating <= 5);
        RatingRecordWithTime *rr = new RatingRecordWithTime(userId, itemId, rating, timestamp);
        mRatings.push_back(rr);
    }
    mNumRating = mRatings.size();
    // sort by rating's timestamp
    std::sort(mRatings.begin(), mRatings.end(),
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
    std::ifstream fin;
    std::string itemInfoFile = mInputDirpath + "/ml-1m/movies.dat";
    fin.open(itemInfoFile.c_str());
    assert(!fin.fail());
    mMovies.clear();
    std::string line;
    while (std::getline(fin, line)) {
        std::vector<std::string> lineFields = StringHelper::Split(line, "::", false);
        Movie movie;
        movie.id = Integer::ParseInt(lineFields[0]);
        movie.title = PrepareDataset1M_ExtractTitle(lineFields[1]);
        movie.year = PrepareDataset1M_ExtractYear(lineFields[1]);
        mMovies.push_back(movie);
    }
    // sort by movie's distribution year
    std::sort(mMovies.begin(), mMovies.end(),
            [](const Movie& lhs, const Movie& rhs){
        return lhs.year < rhs.year;
    });
}

std::string MovielensPrepare::PrepareDataset1M_ExtractTitle(const std::string& s) {
    auto begPos = 0;
    auto endPos = s.rfind('(');
    assert(endPos != std::string::npos);
    std::string title = s.substr(begPos, endPos - begPos);
    return title;
}

int MovielensPrepare::PrepareDataset1M_ExtractYear(const std::string& s) {
    auto begPos = s.rfind('(');
    auto endPos = s.rfind(')');
    assert(begPos != std::string::npos && endPos != std::string::npos);
    ++begPos;
    std::string yearStr = s.substr(begPos, endPos - begPos);
    return Integer::ParseInt(yearStr);
}

void MovielensPrepare::PrepareDataset1M_ReadRatings(){
    Log::I("recsys", "MovielensPrepare::PrepareDataset1M_ReadRatings()");;
    std::ifstream fin;
    std::string ratingFile = mInputDirpath + "/ml-1m/ratings.dat";
    fin.open(ratingFile.c_str());
    assert(!fin.fail());
    mRatings.clear();
    std::string line;
    while (std::getline(fin, line)) {
        std::vector<std::string> lineFields = StringHelper::Split(line, "::", false);
        assert(lineFields.size() == 4);
        int userId = Integer::ParseInt(lineFields[0]);
        int itemId = Integer::ParseInt(lineFields[1]);
        int rating = Integer::ParseInt(lineFields[2]);
        assert(1 <= rating && rating <= 5);
        int timestamp = Integer::ParseInt(lineFields[3]);
        RatingRecordWithTime *rr = new RatingRecordWithTime(userId, itemId, rating, timestamp);
        mRatings.push_back(rr);
    }
    mNumRating = mRatings.size();
    // sort by rating's timestamp
    std::sort(mRatings.begin(), mRatings.end(),
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
    std::ifstream fin;
    std::string itemInfoFile = mInputDirpath + "/ml-10M100K/movies.dat";
    fin.open(itemInfoFile.c_str());
    assert(!fin.fail());
    mMovies.clear();
    std::string line;
    while (std::getline(fin, line)) {
        std::vector<std::string> lineFields = StringHelper::Split(line, "::", false);
        Movie movie;
        movie.id = Integer::ParseInt(lineFields[0]);
        movie.title = PrepareDataset10M_ExtractTitle(lineFields[1]);
        movie.year = PrepareDataset10M_ExtractYear(lineFields[1]);
        mMovies.push_back(movie);
    }
    // sort by movie's distribution year
    std::sort(mMovies.begin(), mMovies.end(),
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
    std::ifstream fin;
    std::string ratingFile = mInputDirpath + "/ml-10M100K/ratings.dat";
    fin.open(ratingFile.c_str());
    assert(!fin.fail());
    mRatings.clear();
    std::string line;
    while (std::getline(fin, line)) {
        std::vector<std::string> lineFields = StringHelper::Split(line, "::", false);
        assert(lineFields.size() == 4);
        int userId = Integer::ParseInt(lineFields[0]);
        int itemId = Integer::ParseInt(lineFields[1]);
        double rating = Double::ParseDouble(lineFields[2]);
        assert(0.0 <= rating && rating <= 5.0);
        int timestamp = Integer::ParseInt(lineFields[3]);
        RatingRecordWithTime *pRating = new RatingRecordWithTime(userId, itemId, rating, timestamp);
        mRatings.push_back(pRating);
    }
    mNumRating = mRatings.size();
    // sort by rating's timestamp
    std::sort(mRatings.begin(), mRatings.end(),
            [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs) {
        return lhs->Timestamp() < rhs->Timestamp();
    });
}

void MovielensPrepare::GenerateUserIdMapping() {
    Log::I("recsys", "MovielensPrepare::GenerateUserIdMapping()");
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

    std::string filename = mOutputDirpath + "/user_id_mapping.txt";
    std::ofstream fout(filename.c_str());
    assert(!fout.fail());
    fout << mNumUser << std::endl;
    for (auto it = mUserIdMap.begin(); it != mUserIdMap.end(); ++it) {
        fout << it->first << ',' << it->second << std::endl;
    }
}

void MovielensPrepare::GenerateItemIdMapping() {
    Log::I("recsys", "MovielensPrepare::GenerateItemIdMapping()");
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

    std::string filename = mOutputDirpath + "/item_id_mapping.txt";
    std::ofstream fout(filename.c_str());
    assert(!fout.fail());
    fout << mNumItem << std::endl;
    for (auto it = mItemIdMap.begin(); it != mItemIdMap.end(); ++it) {
       fout << it->first << "," << it->second << std::endl;
    }
}

void MovielensPrepare::GenerateMovieData() {
    Log::I("recsys", "MovielensPrepare::GenerateMovieData()");
    std::string filename = mOutputDirpath + "/movie.txt";
    std::ofstream fout(filename.c_str());
    assert(!fout.fail());

    int countValidMovie = 0;
    for (auto& movie : mMovies) {
        if (mItemIdMap.find(movie.id) != mItemIdMap.end()) {
            ++countValidMovie;
        }    
    }

    fout << countValidMovie << std::endl;
    for (auto& movie : mMovies) {
        if (mItemIdMap.find(movie.id) != mItemIdMap.end()) {
            fout << mItemIdMap[movie.id] << "," 
                 << movie.year << ","
                 << movie.title << std::endl;
        }
    }
}

void MovielensPrepare::GenerateRatingData() {
    Log::I("recsys", "MovielensPrepare::GenerateRatingData()");
    std::string filenameRatingData = mOutputDirpath + "/rating.txt";
    Log::I("recsys", "writing ratings to file = " + filenameRatingData);
    std::ofstream fout(filenameRatingData.c_str());
    assert(!fout.fail());
    fout << mNumRating << "," << mNumUser << "," << mNumItem << std::endl;
    for (int i = 0; i < mNumRating; ++i) {
        RatingRecordWithTime& rating = *mRatings[i];
        fout << mUserIdMap[rating.UserId()] << ","
             << mItemIdMap[rating.ItemId()] << ","
             << rating.Rating() << "," << rating.Timestamp() << std::endl;
    }
    if (mConfig["splitMethod"].asString() == "byAll") {
        GenerateRatingDataBySplitAll();
    } else if (mConfig["splitMethod"].asString() == "byUser") {
        GenerateRatingDataBySplitUser();
    } else {
        throw LonganConfigError();
    }
}

void MovielensPrepare::GenerateRatingDataBySplitAll() {
    ArrayHelper::RandomShuffle(&mRatings[0], mRatings.size());
    double trainRatio = mConfig["trainRatio"].asDouble();
    int splitPos = static_cast<int>(mNumRating * trainRatio);
    
    std::string filenameTrain = mOutputDirpath + "/rating_train.txt";
    Log::I("recsys", "writing train ratings to file = " + filenameTrain);
    std::ofstream fout1(filenameTrain.c_str());
    assert(!fout1.fail());
    std::sort(&mRatings[0], &mRatings[splitPos],
        [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs) {
            return lhs->Timestamp() < rhs->Timestamp();
        });
    int numTrainRatings = splitPos;
    fout1 << numTrainRatings << ","
          << mNumUser << ","
          << mNumItem << std::endl;
    for (int i = 0; i < splitPos; ++i) {
        RatingRecordWithTime& rating = *mRatings[i];
        fout1 << mUserIdMap[rating.UserId()] << "," 
              << mItemIdMap[rating.ItemId()] << ","
              << rating.Rating() << "," 
              << rating.Timestamp() << std::endl;
    }

    std::string filenameTest = mOutputDirpath + "/rating_test.txt";
    Log::I("recsys", "writing test ratings to file: " + filenameTest);
    std::ofstream fout2(filenameTest.c_str());
    assert(!fout2.fail());
    std::sort(&mRatings[splitPos], &mRatings[0] + mRatings.size(),
        [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs) {
            return lhs->Timestamp() < rhs->Timestamp();
        });
    int numTestRatings = mRatings.size() - numTrainRatings;
    fout2 << numTestRatings << ","
          << mNumUser << ","
          << mNumItem << std::endl;
    for (int i = splitPos; i < mRatings.size(); ++i) {
        RatingRecordWithTime& rating = *mRatings[i];
        fout2 << mUserIdMap[rating.UserId()] << "," 
              << mItemIdMap[rating.ItemId()] << ","
              << rating.Rating() << "," 
              << rating.Timestamp() << std::endl;
    }
}

void MovielensPrepare::GenerateRatingDataBySplitUser() {
    QuickSortMT sort;
    sort(mRatings.data(), mRatings.size(),
            [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs)->int {
        if (lhs->UserId() != rhs->UserId()) {
            return lhs->UserId() - rhs->UserId();
        } else {
            return lhs->ItemId() - rhs->ItemId();
        }
    });
    std::vector<RatingRecordWithTime*> trainRatings;
    std::vector<RatingRecordWithTime*> testRatings;
    double trainRatio = mConfig["trainRatio"].asDouble();
    for (int i = 0; i < mNumRating; ++i) {
        int begin = i;
        while (i+1 < mNumRating && mRatings[i]->UserId() == mRatings[i+1]->UserId()) {
            ++i;
        }
        RatingRecordWithTime **data = &mRatings[begin];
        int size = i - begin + 1;
        ArrayHelper::RandomShuffle(data, size);
        int splitPos = static_cast<int>(size * trainRatio);
        for (int j = 0; j < splitPos; ++j) {
            trainRatings.push_back(data[j]);
        }
        for (int j = splitPos; j < size; ++j) {
            testRatings.push_back(data[j]);
        }
    }
    assert(trainRatings.size() + testRatings.size()  == mNumRating);

    std::string filenameTrain = mOutputDirpath + "/rating_train.txt";
    Log::I("recsys", "writing train ratings to file = " + filenameTrain);
    std::ofstream fout1(filenameTrain.c_str());
    assert(!fout1.fail());
    fout1 << trainRatings.size() << "," << mNumUser << "," << mNumItem << std::endl;
    for (int i = 0; i < trainRatings.size(); ++i) {
        RatingRecordWithTime& rating = *trainRatings[i];
        fout1 << mUserIdMap[rating.UserId()] << ","
              << mItemIdMap[rating.ItemId()] << ","
              << rating.Rating() << "," << rating.Timestamp() << std::endl;
    }

    std::string filenameTest = mOutputDirpath + "/rating_test.txt";
    Log::I("recsys", "writing test ratings to file: " + filenameTest);
    std::ofstream fout2(filenameTest.c_str());
    assert(!fout2.fail());
    fout2 << testRatings.size() << "," << mNumUser << "," << mNumItem << std::endl;
    for (int i = 0; i < testRatings.size(); ++i) {
        RatingRecordWithTime& rating = *testRatings[i];
        fout2 << mUserIdMap[rating.UserId()] << ","
              << mItemIdMap[rating.ItemId()] << ","
              << rating.Rating() << "," << rating.Timestamp() << std::endl;
    }
}

void MovielensPrepare::Cleanup() {
    Log::I("recsys", "MovielensPrepare::Cleanup()");
    for (RatingRecordWithTime* pRating : mRatings) {
        delete pRating;
    }
}

} //~ namespace longan

