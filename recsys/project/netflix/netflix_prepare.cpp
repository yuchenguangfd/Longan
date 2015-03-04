/*
 * netflix_prepare.cpp
 * Created on: Aug 5, 2014
 * Author: chenguangyu
 */

#include "netflix_prepare.h"
#include "algorithm/sort/quick_sort_mt.h"
#include "common/common.h"

namespace longan {

NetflixPrepare::NetflixPrepare(const std::string& inputDirpath, const std::string& configFilepath,
        const std::string& outputDirpath) :
    mInputDirpath(inputDirpath), mConfigFilepath(configFilepath), mOutputDirpath(outputDirpath),
    mNumUser(0), mNumItem(0), mNumRating(0),
    mNumTestRatings(0) { }

void NetflixPrepare::Prepare() {
    Log::I("recsys", "NetflixPrepare::Prepare()");
    LoadConfig();
    ReadItemInfo();
    ReadAllRating();
    GenerateUserIdMapping();
    GenerateItemIdMapping();
    GenerateMovieData();
    GenerateRatingData();
    Cleanup();
}

void NetflixPrepare::LoadConfig() {
    Log::I("recsys", "NetflixPrepare::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    JsonConfigHelper::LoadFromFile(mConfigFilepath, mConfig);
}

void NetflixPrepare::ReadItemInfo() {
    Log::I("recsys", "NetflixPrepare::ReadItemInfo()");
    std::ifstream fin;
    std::string itemInfoFile = mInputDirpath + "/movie_titles.txt";
    fin.open(itemInfoFile.c_str());
    assert(!fin.fail());

    mMovies.clear();
    std::string line;
    while (std::getline(fin, line)) {
        std::vector<std::string> lineFields = StringHelper::Split(line, ",", false);
        Movie movie;
        movie.id = Integer::ParseInt(lineFields[0]);
        movie.year = Integer::ParseInt(lineFields[1]);
        movie.title = lineFields[2];
        mMovies.push_back(movie);
    }
    // sort by movie's distribution year
    std::sort(mMovies.begin(), mMovies.end(),
            [](const Movie& lhs, const Movie& rhs)->bool {
        return lhs.year < rhs.year;
    });
}

void NetflixPrepare::ReadAllRating() {
    Log::I("recsys", "NetflixPrepare::ReadAllRating()");
    std::string ratingDir = mInputDirpath + "/training_set";
    FileLister lister(ratingDir);
    std::vector<std::string> ratingFiles = lister.ListFilename();
    mRatings.clear();
    for (int i = 0; i < ratingFiles.size(); ++i) {
        if (i % 1000 == 0) Log::I("recsys", "loading rating data file %d/%d...", i, ratingFiles.size());
        ReadRatingFile(ratingFiles[i]);
    }
    mNumRating = mRatings.size();
    Log::I("recsys", "num rating = %d", mRatings.size());
    Log::I("recsys", "sorting all ratings by timestamp...");
    QuickSortMT sort;
    sort(&mRatings[0], mRatings.size(),
         [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs)->int {
        return (lhs->Timestamp() - rhs->Timestamp());
    });
}

void NetflixPrepare::ReadRatingFile(const std::string& filename) {
    int itemId = GetItemIdFromFileName(filename);
    std::string ratingFile = mInputDirpath + "/training_set/" + filename;
    std::ifstream fin;
    fin.open(ratingFile.c_str());
    assert(!fin.fail());

    std::string line;
    std::getline(fin, line);
    assert(Integer::ParseInt(line) == itemId);
    while (std::getline(fin, line)) {
        std::vector<std::string> lineFields = StringHelper::Split(line, ",");
        int userId = Integer::ParseInt(lineFields[0]);
        float rating = Integer::ParseInt(lineFields[1]);
        int timestamp = GetTimestampFromDate(lineFields[2]);
        RatingRecordWithTime *pRating = new RatingRecordWithTime(userId, itemId, rating, timestamp);
        mRatings.push_back(pRating);
    }
}

int NetflixPrepare::GetItemIdFromFileName(const std::string& filename) {
    auto begPos = filename.find('_') + 1;
    auto endPos = filename.find('.');
    assert(begPos != std::string::npos);
    assert(endPos != std::string::npos);
    assert(begPos < endPos);
    return Integer::ParseInt(filename.substr(begPos, endPos - begPos));
}

int NetflixPrepare::GetTimestampFromDate(const std::string& date) {
    // date has the format YYYY-MM-DD.
    return Integer::ParseInt(date.substr(0, 4) + date.substr(5, 2) + date.substr(8, 2));
}

void NetflixPrepare::GenerateUserIdMapping() {
    Log::I("recsys", "NetflixPrepare::GenerateUserIdMapping()");
    mUserIdMap.clear();
    int userCount = 0;
    for (RatingRecordWithTime *pRating : mRatings) {
        if (mUserIdMap.find(pRating->UserId()) == mUserIdMap.end()) {
            mUserIdMap[pRating->UserId()] = userCount;
            ++userCount;
        }
    }
    mNumUser = userCount;

    std::string filename = mOutputDirpath + "/user_id_mapping.txt";
    std::ofstream fout;
    fout.open(filename.c_str());
    assert(!fout.fail());
    fout << mNumUser << std::endl;
    for (auto it = mUserIdMap.begin(); it != mUserIdMap.end(); ++it) {
        fout << it->first << "," << it->second << std::endl;
    }
}

void NetflixPrepare::GenerateItemIdMapping() {
    Log::I("recsys", "NetflixPrepare::GenerateItemIdMapping()");
    mItemIdMap.clear();
    int itemCount = 0;
    for (RatingRecordWithTime *pRating : mRatings) {
        if (mItemIdMap.find(pRating->ItemId()) == mItemIdMap.end()) {
            mItemIdMap[pRating->ItemId()] = itemCount;
            ++itemCount;
        }
    }
    mNumItem = itemCount;

    std::string filename = mOutputDirpath + "/item_id_mapping.txt";
    std::ofstream fout;
    fout.open(filename.c_str());
    assert(!fout.fail());
    fout << mNumItem << std::endl;
    for (auto it = mItemIdMap.begin(); it != mItemIdMap.end(); ++it) {
       fout << it->first << "," << it->second << std::endl;
    }
}

void NetflixPrepare::GenerateMovieData() {
    Log::I("recsys", "NetflixPrepare::GenerateMovieData()");
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

void NetflixPrepare::GenerateRatingData() {
    Log::I("recsys", "NetflixPrepare::GenerateRatingData()");
    if (mConfig["testDataFrom"].asString() == "probe") {
        ReadTestRating();
        SetTrainOrTestFlag();
        GenerateTrainRatings();
        GenerateTestRatings();
    } else if (mConfig["testDataFrom"].asString() == "randomSelect") {
        GenerateTrainAndTestRatings();
    } else {
        throw LonganConfigError();
    }
}

void NetflixPrepare::ReadTestRating() {
    Log::I("recsys", "NetflixPrepare::ReadTestRating()");
    std::string filename = mInputDirpath + "/probe.txt";
    std::ifstream fin(filename.c_str());
    assert(!fin.fail());

    mTestRatingSet.clear();
    std::string line;
    int userId, itemId;
    while(std::getline(fin, line)) {
        if (line.empty()) continue;
        if (line.find(':') != std::string::npos) {
            itemId = Integer::ParseInt(line);
            assert(mItemIdMap.find(itemId) != mItemIdMap.end());
        } else {
            userId = Integer::ParseInt(line);
            assert(mUserIdMap.find(userId) != mUserIdMap.end());
            UserItemPair pair;
            pair.userId = userId;
            pair.itemId = itemId;
            mTestRatingSet.insert(pair);
        }
    }
}

void NetflixPrepare::SetTrainOrTestFlag() {
    Log::I("recsys", "NetflixPrepare::SetTrainOrTestFlag()");
    mIsTestRatingFlags.resize(mRatings.size());
    mNumTestRatings = 0;
    for (int i = 0; i < mRatings.size(); ++i) {
        RatingRecordWithTime& rating = *mRatings[i];
        UserItemPair pair;
        pair.userId = rating.UserId();
        pair.itemId = rating.ItemId();
        mIsTestRatingFlags[i] = (mTestRatingSet.find(pair) != mTestRatingSet.end());
        if (mIsTestRatingFlags[i]) ++mNumTestRatings;
    }
}

void NetflixPrepare::GenerateTrainRatings() {
    Log::I("recsys", "NetflixPrepare::GenerateTrainRatings()");
    std::string filename = mOutputDirpath + "/rating_train.txt";
    std::ofstream fout(filename.c_str());
    assert(!fout.fail());
    int numTrainRatings = mNumRating - mNumTestRatings;
    fout << numTrainRatings << ","
         << mNumUser << ","
         << mNumItem << std::endl;
    for (int i = 0; i < mRatings.size(); ++i) {
        if (mIsTestRatingFlags[i]) continue;
        RatingRecordWithTime& rating = *mRatings[i];
        fout << mUserIdMap[rating.UserId()] << ","
             << mItemIdMap[rating.ItemId()] << ","
             << rating.Rating() << "," << rating.Timestamp() << std::endl;
    }
}

void NetflixPrepare::GenerateTestRatings() {
    Log::I("recsys", "NetflixPrepare::GenerateTestRatings()");
    std::string filename = mOutputDirpath + "/rating_test.txt";
    std::ofstream fout(filename.c_str());
    assert(!fout.fail());
    fout << mNumTestRatings << ","
         << mNumUser << ","
         << mNumItem << std::endl;;
    for (int i = 0; i < mRatings.size(); ++i) {
        if (!mIsTestRatingFlags[i]) continue;
        RatingRecordWithTime& rating = *mRatings[i];
        fout << mUserIdMap[rating.UserId()] << ","
             << mItemIdMap[rating.ItemId()] << ","
             << rating.Rating() << "," << rating.Timestamp() << std::endl;
    }
}

void NetflixPrepare::GenerateTrainAndTestRatings() {
    Log::I("recsys", "NetflixPrepare::GenerateTrainAndTestRatings()");
    ArrayHelper::RandomShuffle(&mRatings[0], mRatings.size());
    double trainRatio = mConfig["trainRatio"].asDouble();
    assert(trainRatio > 0.0);
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

void NetflixPrepare::Cleanup() {
    for (RatingRecordWithTime* pRating : mRatings) {
        delete pRating;
    }
}

} //~ namespace longan
