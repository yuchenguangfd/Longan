/*
 * netflix_prepare.cpp
 * Created on: Aug 5, 2014
 * Author: chenguangyu
 */

#include "netflix_prepare.h"
#include "common/logging/logging.h"
#include "common/system/file_lister.h"
#include "common/util/string_helper.h"
#include "common/lang/integer.h"
#include "common/error.h"
#include "algorithm/sort/quick_sort_mt.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <utility>
#include <cassert>

namespace longan {

NetflixPrepare::NetflixPrepare(const std::string& inputDirpath, const std::string& outputDirpath) :
    mInputDirpath(inputDirpath), mOutputDirpath(outputDirpath),
    mNumUser(0), mNumItem(0), mNumRating(0),
    mNumTestRatings(0) { }

void NetflixPrepare::Prepare() {
    Log::I("recsys", "NetflixPrepare::Prepare()");
    ReadItemInfo();
    ReadAllRating();
    GenerateUserIdMapping();
    GenerateItemIdMapping();
    GenerateMovieData();
    GenerateRatingData();
    Cleanup();
}

void NetflixPrepare::ReadItemInfo() {
    Log::I("recsys", "NetflixPrepare::ReadItemInfo()");
    using namespace std;
    ifstream fin;
    string itemInfoFile = mInputDirpath + "/movie_titles.txt";
    fin.open(itemInfoFile.c_str());
    assert(!fin.fail());

    mMovies.clear();
    string line;
    while (getline(fin, line)) {
        vector<string> lineFields = StringHelper::Split(line, ",", false);
        Movie movie;
        movie.id = Integer::ParseInt(lineFields[0]);
        movie.year = Integer::ParseInt(lineFields[1]);
        movie.title = lineFields[2];
        mMovies.push_back(movie);
    }
    // sort by movie's distribution year
    sort(mMovies.begin(), mMovies.end(),
            [](const Movie& lhs, const Movie& rhs)->bool {
        return lhs.year < rhs.year;
    });
}

void NetflixPrepare::ReadAllRating() {
    Log::I("recsys", "NetflixPrepare::ReadAllRating()");
    using namespace std;
    using namespace StringHelper;
    string ratingDir = mInputDirpath + "/training_set";
    FileLister lister(ratingDir);
    vector<string> ratingFiles = lister.ListFilename();
    mRatings.clear();
    for (int i = 0; i < ratingFiles.size(); ++i) {
        if (i % 1000 == 0) Log::I("recsys", "loading rating data file " + ToString(i) + "/" + ToString(ratingFiles.size()));
        ReadRatingFile(ratingFiles[i]);
    }
    mNumRating = mRatings.size();
    Log::I("recsys", "num rating = " + ToString(mRatings.size()));
    Log::I("recsys", "sorting all ratings by timestamp...");
    QuickSortMT sort;
    sort(&mRatings[0], mRatings.size(),
         [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs)->int {
        return (lhs->Timestamp() - rhs->Timestamp());
    });
}

void NetflixPrepare::ReadRatingFile(const std::string& filename) {
    using namespace std;
    int itemId = GetItemIdFromFileName(filename);
    string ratingFile = mInputDirpath + "/training_set/" + filename;
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

    string filename = mOutputDirpath + "/user_id_mapping.txt";
    ofstream fout;
    fout.open(filename.c_str());
    assert(!fout.fail());
    fout << mNumUser << endl;
    for (auto it = mUserIdMap.begin(); it != mUserIdMap.end(); ++it) {
        fout << it->first << "," << it->second << endl;
    }
}

void NetflixPrepare::GenerateItemIdMapping() {
    Log::I("recsys", "NetflixPrepare::GenerateItemIdMapping()");
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

    string filename = mOutputDirpath + "/item_id_mapping.txt";
    ofstream fout;
    fout.open(filename.c_str());
    assert(!fout.fail());
    fout << mNumItem << endl;
    for (auto it = mItemIdMap.begin(); it != mItemIdMap.end(); ++it) {
       fout << it->first << "," << it->second << endl;
    }
}

void NetflixPrepare::GenerateMovieData() {
    Log::I("recsys", "NetflixPrepare::GenerateMovieData()");
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

void NetflixPrepare::GenerateRatingData() {
    Log::I("recsys", "NetflixPrepare::GenerateRatingData()");
    ReadTestRating();
    SetTrainOrTestFlag();
    GenerateTrainRatings();
    GenerateTestRatings();
}

void NetflixPrepare::ReadTestRating() {
    Log::I("recsys", "NetflixPrepare::ReadTestRating()");
    using namespace std;
    string filename = mInputDirpath + "/probe.txt";
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

void NetflixPrepare::SetTrainOrTestFlag() {
    Log::I("recsys", "NetflixPrepare::SetTrainOrTestFlag()");
    using namespace std;
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
    using namespace std;
    string filename = mOutputDirpath + "/rating_train.txt";
    ofstream fout(filename.c_str());
    assert(!fout.fail());
    int numTrainRatings = mNumRating - mNumTestRatings;
    fout << numTrainRatings << ","
         << mNumUser << ","
         << mNumItem << endl;
    for (int i = 0; i < mRatings.size(); ++i) {
        if (mIsTestRatingFlags[i]) continue;
        RatingRecordWithTime& rating = *mRatings[i];
        fout << mUserIdMap[rating.UserId()] << ","
             << mItemIdMap[rating.ItemId()] << ","
             << rating.Rating() << "," << rating.Timestamp() << endl;
    }
}

void NetflixPrepare::GenerateTestRatings() {
    Log::I("recsys", "NetflixPrepare::GenerateTestRatings()");
    using namespace std;
    string filename = mOutputDirpath + "/rating_test.txt";
    ofstream fout(filename.c_str());
    assert(!fout.fail());
    fout << mNumTestRatings << ","
         << mNumUser << ","
         << mNumItem << endl;;
    for (int i = 0; i < mRatings.size(); ++i) {
        if (!mIsTestRatingFlags[i]) continue;
        RatingRecordWithTime& rating = *mRatings[i];
        fout << mUserIdMap[rating.UserId()] << ","
             << mItemIdMap[rating.ItemId()] << ","
             << rating.Rating() << "," << rating.Timestamp() << endl;
    }
}

void NetflixPrepare::Cleanup() {
    for (RatingRecordWithTime* pRating : mRatings) {
        delete pRating;
    }
}

} //~ namespace longan
