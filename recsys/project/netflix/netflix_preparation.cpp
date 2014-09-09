/*
 * netflix_preparation.cpp
 * Created on: Aug 5, 2014
 * Author: chenguangyu
 */

#include "netflix_preparation.h"
#include "common/util/string_helper.h"
#include "common/lang/integer.h"
#include "common/system/file_lister.h"
#include "common/error.h"
#include "common/logging/logging_helper.h"
#include "algorithm/sort/quick_sort_mt.h"
#include <glog/logging.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <utility>
#include <cassert>

namespace longan {

NetflixPreparation::NetflixPreparation(const std::string& inputPath, const std::string& outputPath) :
    mInputPath(inputPath), mOutputPath(outputPath),
    mNumUser(0), mNumItem(0), mNumRating(0),
    mNumTestRatings(0) { }

void NetflixPreparation::PrepareDataset() {
    LOG_FUNC;
    ReadItemInfo();
    ReadAllRating();
    GenerateUserIdMapping();
    GenerateItemIdMapping();
    GenerateMovieData();
    GenerateRatingData();
    FreeRatings();
}

void NetflixPreparation::ReadItemInfo() {
    LOG_FUNC;
    using namespace std;
    ifstream fin;
    string itemInfoFile = mInputPath + "/movie_titles.txt";
    fin.open(itemInfoFile.c_str());
    assert(!fin.fail());

    mMovies.clear();
    string line;
    while (getline(fin, line)) {
        vector<string> lineFields = StringHelper::Split(line, ",", false);
        Movie movie;
        movie.id = Integer::ParseInt(lineFields[0]);
        movie.year = Integer::ParseInt(lineFields[1]);
        movie.title = Integer::ParseInt(lineFields[2]);
        mMovies.push_back(movie);
    }
    // sort by movie's distribution year
    sort(mMovies.begin(), mMovies.end(),
            [](const Movie& lhs, const Movie& rhs)->bool {
        return lhs.year < rhs.year;
    });
}

void NetflixPreparation::ReadAllRating() {
    LOG_FUNC;
    using namespace std;
    string ratingDir = mInputPath + "/training_set";
    FileLister lister(ratingDir);
    vector<string> ratingFiles = lister.ListFilename();
    mRatings.clear();
    for (int i = 0; i < ratingFiles.size(); ++i) {
        LOG_IF(INFO, i % 1000 == 0) << "loading rating data file " << i << "/" << ratingFiles.size();
        ReadRatingFile(ratingFiles[i]);
    }
    mNumRating = mRatings.size();
    LOG(INFO) << "#rating = " << mRatings.size();
    LOG(INFO) << "sorting all ratings by timestamp...";
    QuickSortMT sort;
    sort(&mRatings[0], mRatings.size(),
         [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs)->int {
        return (lhs->Timestamp() - rhs->Timestamp());
    });
}

void NetflixPreparation::ReadRatingFile(const std::string& filename) {
    using namespace std;
    int itemId = GetItemIdFromFileName(filename);
    string ratingFile = mInputPath + "/training_set/" + filename;
    ifstream fin;
    fin.open(ratingFile.c_str());
    assert(!fin.fail());

    string line;
    getline(fin, line);
    assert(Integer::ParseInt(line) == itemId);
    while (getline(fin, line)) {
        vector<string> lineFields = StringHelper::Split(line, ",");
        int userId = Integer::ParseInt(lineFields[0]);
        float rating = Integer::ParseInt(lineFields[1]);
        int timestamp = GetTimestampFromDate(lineFields[2]);
        auto pRating = new RatingRecordWithTime(userId, itemId, rating, timestamp);
        mRatings.push_back(pRating);
    }
}

void NetflixPreparation::GenerateUserIdMapping() {
    LOG_FUNC;
    using namespace std;
    mUserIdMap.clear();
    int userCount = 0;
    for (RatingRecordWithTime *pRating : mRatings) {
        if (mUserIdMap.find(pRating->UserId()) == mUserIdMap.end()) {
            mUserIdMap[pRating->UserId()] = userCount;
            ++userCount;
        }
    }
    mNumUser = userCount;

    string filename = mOutputPath + "/user_id_mapping.txt";
    ofstream fout;
    fout.open(filename.c_str());
    assert(!fout.fail());
    fout << mNumUser << endl;
    for (auto it = mUserIdMap.begin(); it != mUserIdMap.end(); ++it) {
        fout << it->first << "," << it->second << endl;
    }
}

void NetflixPreparation::GenerateItemIdMapping() {
    LOG_FUNC;
    using namespace std;
    mItemIdMap.clear();
    int itemCount = 0;
    for (RatingRecordWithTime *pRating : mRatings) {
        if (mItemIdMap.find(pRating->ItemId()) == mItemIdMap.end()) {
            mItemIdMap[pRating->ItemId()] = itemCount;
            ++itemCount;
        }
    }
    mNumItem = itemCount;

    string filename = mOutputPath + "/item_id_mapping.txt";
    ofstream fout;
    fout.open(filename.c_str());
    assert(!fout.fail());
    fout << mNumItem << endl;
    for (auto it = mItemIdMap.begin(); it != mItemIdMap.end(); ++it) {
       fout << it->first << "," << it->second << endl;
    }
}

void NetflixPreparation::GenerateMovieData() {
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

void NetflixPreparation::GenerateRatingData() {
    LOG_FUNC;
    ReadTestRating();
    SetTrainOrTestFlag();
    GenerateTrainRatings();
    GenerateTestRatings();
}

void NetflixPreparation::ReadTestRating() {
    LOG_FUNC;
    using namespace std;
    string filename = mInputPath + "/probe.txt";
    ifstream fin(filename.c_str());
    assert(!fin.fail());

    mTestRatingSet.clear();
    string line;
    int userId, itemId;
    while(getline(fin, line)) {
        if (line.empty()) continue;
        if (line.find(':') != string::npos) {
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

void NetflixPreparation::SetTrainOrTestFlag() {
    LOG_FUNC;
    using namespace std;
    mIsTestRatingFlags.resize(mRatings.size());
    mNumTestRatings = 0;
    for (int i = 0; i < mRatings.size(); ++i) {
        LOG_IF(INFO, i % 10000000 == 0) << "process" << i << "/" << mRatings.size();
        RatingRecordWithTime& rating = *mRatings[i];
        UserItemPair pair;
        pair.userId = rating.UserId();
        pair.itemId = rating.ItemId();
        mIsTestRatingFlags[i] = (mTestRatingSet.find(pair) != mTestRatingSet.end());
        if (mIsTestRatingFlags[i]) ++mNumTestRatings;
    }
}

void NetflixPreparation::GenerateTrainRatings() {
    LOG_FUNC;
    using namespace std;
    string filename = mOutputPath + "/rating_train.txt";
    ofstream fout(filename.c_str());
    assert(!fout.fail());
    int numTrainRatings = mNumRating - mNumTestRatings;
    fout << numTrainRatings << endl;
    for (int i = 0; i < mRatings.size(); ++i) {
        LOG_IF(INFO, i % 10000000 == 0) << "process" << i << "/" << mRatings.size();
        if (mIsTestRatingFlags[i]) continue;
        RatingRecordWithTime& rating = *mRatings[i];
        fout << mUserIdMap[rating.UserId()] << ","
             << mItemIdMap[rating.ItemId()] << ","
             << rating.Rating() << "," << rating.Timestamp() << endl;
    }
}

void NetflixPreparation::GenerateTestRatings() {
    LOG_FUNC;
    using namespace std;
    string filename = mOutputPath + "/rating_test.txt";
    ofstream fout(filename.c_str());
    assert(!fout.fail());
    fout << mNumTestRatings << endl;
    for (int i = 0; i < mRatings.size(); ++i) {
        LOG_IF(INFO, i % 10000000 == 0) << "process" << i << "/" << mRatings.size();
        if (!mIsTestRatingFlags[i]) continue;
        RatingRecordWithTime& rating = *mRatings[i];
        fout << mUserIdMap[rating.UserId()] << ","
             << mItemIdMap[rating.ItemId()] << ","
             << rating.Rating() << "," << rating.Timestamp() << endl;
    }
}

void NetflixPreparation::FreeRatings() {
    LOG_FUNC;
    for (RatingRecordWithTime* pRating : mRatings) {
        delete pRating;
    }
}

int NetflixPreparation::GetItemIdFromFileName(const std::string& filename) {
    auto begPos = filename.find('_') + 1;
    auto endPos = filename.find('.');
    assert(begPos != std::string::npos);
    assert(endPos != std::string::npos);
    assert(begPos < endPos);
    return Integer::ParseInt(filename.substr(begPos, endPos - begPos));
}

int NetflixPreparation::GetTimestampFromDate(const std::string& date) {
    // date has the format YYYY-MM-DD.
    return Integer::ParseInt(date.substr(0, 4) + date.substr(5, 2) + date.substr(8, 2));
}

} //~ namespace longan
