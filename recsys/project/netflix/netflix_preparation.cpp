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
    mNumTestRatings(0) { }

/*
 * output1: movie.txt
 * file format:
 * Line_1: #movies
 * Line_i: ID, Year, Title (one movie each line, sort by Year)
 *
 * output2: item_id_mapping.txt
 * file format:
 * Line_1: #movies
 * Line_i: original movie ID(string type), mapped movie ID(integer from 0 to #movies-1)
 */
void NetflixPreparation::PrepareMovieData() {
    using namespace std;
    string inputMovieFile = mInputPath + "/movie_titles.txt";
    LOG(INFO) << "open movie data file:" << inputMovieFile;
    ifstream fin(inputMovieFile);
    assert(!fin.fail());

    LOG(INFO) << "loading movie data";
    vector<Movie*> movies;
    string line;
    while (getline(fin, line)) {
        vector<string> subs = StringHelper::Split(line, ",");
        Movie *pMovie = new Movie();
        pMovie->id = subs[0];
        pMovie->title = subs[2];
        pMovie->year = Integer::ParseInt(subs[1]);
        movies.push_back(pMovie);
    }

    LOG(INFO) << "sort movie data by year";
    sort(movies.begin(), movies.end(),
        [](Movie *lhs, Movie *rhs) -> int {
            return (lhs->year < rhs->year);
        });

    string outputMovieFile = mOutputPath + "/movie.txt";
    LOG(INFO) << "writing movie data to file: " << outputMovieFile;
    ofstream fout1(outputMovieFile);
    assert(!fout1.fail());
    fout1 << movies.size() << endl;
    for (int i = 0; i < movies.size(); ++i) {
        fout1 << movies[i]->id << "," << movies[i]->year << "," << movies[i]->title << endl;
    }

    string outputMappingFile = mOutputPath + "/item_id_mapping.txt";
    LOG(INFO) << "writing movie id mapping to file: " << outputMappingFile;
    ofstream fout2(outputMappingFile);
    assert(!fout2.fail());
    fout2 << movies.size() << endl;
    for (int i = 0; i < movies.size(); ++i) {
        fout2 << movies[i]->id << "," << i << endl;
    }

    for (Movie* pMovie : movies) {
        delete pMovie;
    }
}

/*
 * output1: user_id_mapping.txt
 * file format:
 * line_1: #users
 * line_i: original user ID(string type), mapped user ID(integer from 0 to #users-1)
 *
 * output2: rating_train.txt
 * file format:
 * line_1: #ratings
 * line_i: user ID, item ID, rating, timestamp
 * user ID is integer from 0 to #users-1, item ID is integer from 0 to #movies-1
 *
 * output3: rating_test.txt
 * file format:
 * same as rating_train.txt
 */
void NetflixPreparation::PrepareRatingData() {
    LoadAllRatings();
    SortAllRatingsByTime();
    GenerateUserIdMapping();
    GenerateTrainAndTestRatings();
    FreeAllRatings();
}

void NetflixPreparation::LoadAllRatings() {
    using namespace std;
    LOG(INFO) << "loading all ratings.";
    mAllRatings.clear();
    FileLister lister(mInputPath + "/training_set");
    vector<string> ratingFiles = lister.ListFilename();
    for (string& filename : ratingFiles) {
        LoadRatingsFromFile(filename);
    }
    LOG(INFO) << "load all ratings finished. #rating = " << mAllRatings.size();
}

void NetflixPreparation::LoadRatingsFromFile(const std::string& filename) {
    using namespace std;
    int itemId = GetItemIdFromFileName(filename);

    string path = mInputPath + "/training_set/" + filename;
    LOG(INFO) << "loading rating data file:" << path;
    ifstream fin(path);
    assert(!fin.fail());

    string line;
    getline(fin, line);
    assert(Integer::ParseInt(line) == itemId);
    while (getline(fin, line)) {
        vector<string> subs = StringHelper::Split(line, ",");
        int userId = Integer::ParseInt(subs[0]);
        float rating = Integer::ParseInt(subs[1]);
        int time = GetTimestampFromDate(subs[2]);
        mAllRatings.push_back(new RatingRecordWithTime(userId, itemId, rating, time));
    }
}

void NetflixPreparation::SortAllRatingsByTime() {
    LOG(INFO) << "start sorting all ratings by time.";
    QuickSortMT sort;
    sort(&mAllRatings[0], mAllRatings.size(),
         [](RatingRecordWithTime *lhs, RatingRecordWithTime *rhs){
        return (lhs->Timestamp() - rhs->Timestamp());
    });
    LOG(INFO) << "sorting all ratings done.";
}

void NetflixPreparation::GenerateUserIdMapping() {
    using namespace std;
    LOG(INFO) << "generating user id mapping";
    mUserIdMap.clear();
    int userCount = 0;
    for (RatingRecordWithTime *pRating : mAllRatings) {
        if (mUserIdMap.find(pRating->UserId()) == mUserIdMap.end()) {
            mUserIdMap[pRating->UserId()] = userCount;
            ++userCount;
        }
    }

    string filename = mOutputPath + "/user_id_mapping.txt";
    LOG(INFO) << "writing user id mapping to file:" << filename;
    ofstream fout(filename);
    assert(!fout.fail());
    int numUser = mUserIdMap.size();
    fout << numUser << endl;
    for (auto it = mUserIdMap.begin(); it != mUserIdMap.end(); ++it) {
        fout << it->first << "," << it->second << endl;
    }
}

void NetflixPreparation::GenerateTrainAndTestRatings() {
    LoadItemIdMapping();
    LoadTestRatingRecords();
    SetTrainOrTestFlag();
    GenerateTrainRatings();
    GenerateTestRatings();
}

void NetflixPreparation::LoadItemIdMapping() {
    using namespace std;
    string filename = mOutputPath+"/item_id_mapping.txt";
    LOG(INFO) << "loading item id mapping from file: " << filename;
    ifstream fin(filename);
    assert(!fin.fail());
    int numItem;
    fin >> numItem;
    mItemIdMap.clear();
    for (int i = 0; i < numItem; ++i) {
        int oriId, mapId;
        char c;
        fin >> oriId >> c >> mapId;
        mItemIdMap[oriId] = mapId;
    }
}

void NetflixPreparation::LoadTestRatingRecords() {
    using namespace std;
    string filename = mInputPath + "/probe.txt";
    LOG(INFO) << "loading test ratings from file: " << filename;
    ifstream fin(filename);
    assert(!fin.fail());

    mTestRatingSet.clear();
    string line;
    int userId, itemId;
    while(getline(fin, line)) {
        if (line.empty()) continue;
        if (line.find(':') != string::npos) {
            itemId = Integer::ParseInt(line);
            assert(mItemIdMap.find(itemId) != mItemIdMap.end());
            itemId = mItemIdMap[itemId];
            continue;
        }
        userId = Integer::ParseInt(line);
        assert(mUserIdMap.find(userId) != mUserIdMap.end());
        userId = mUserIdMap[userId];
        UserItemPair pair;
        pair.userId = userId;
        pair.itemId = itemId;
        mTestRatingSet.insert(pair);
    }
}

void NetflixPreparation::SetTrainOrTestFlag() {
    using namespace std;
    LOG(INFO) << "changing all ratings' user id and item id to mapped int";
    for (vector<RatingRecordWithTime*>::size_type i = 0; i < mAllRatings.size(); ++i) {
        RatingRecordWithTime& rating = *mAllRatings[i];
        rating.SetUserId(mUserIdMap[rating.UserId()]);
        rating.SetItemId(mItemIdMap[rating.ItemId()]);
    }
    LOG(INFO) << "setting train or test flag";
    mAllRatingsIsTestFlag.resize(mAllRatings.size());
    mNumTestRatings = 0;
    for (vector<RatingRecordWithTime*>::size_type i = 0; i < mAllRatings.size(); ++i) {
        LOG_IF(INFO, i % 5000000 == 0) << "process" << i << "/" << mAllRatings.size();
        RatingRecordWithTime& rating = *mAllRatings[i];
        UserItemPair pair;
        pair.userId = rating.UserId();
        pair.itemId = rating.ItemId();
        mAllRatingsIsTestFlag[i] = (mTestRatingSet.find(pair) != mTestRatingSet.end());
        if (mAllRatingsIsTestFlag[i]) ++mNumTestRatings;
    }
}

void NetflixPreparation::GenerateTrainRatings() {
    using namespace std;
    string filename = mOutputPath + "/rating_train.txt";
    LOG(INFO) << "writing train ratings to file: " << filename;
    ofstream fout(filename);
    assert(!fout.fail());
    int numTrainRatings = mAllRatings.size() - mNumTestRatings;
    fout << numTrainRatings << endl;
    for (vector<RatingRecordWithTime*>::size_type i = 0; i < mAllRatings.size(); ++i) {
        LOG_IF(INFO, i % 5000000 == 0) << "process" << i << "/" << mAllRatings.size();
        if (mAllRatingsIsTestFlag[i]) continue;
        RatingRecordWithTime& rating = *mAllRatings[i];
        fout << rating.UserId() << "," << rating.ItemId() << ","
             << rating.Rating() << "," << rating.Timestamp() << endl;
    }
}

void NetflixPreparation::GenerateTestRatings() {
    using namespace std;
    string filename = mOutputPath + "/rating_test.txt";
    LOG(INFO) << "writing test ratings to file: " << filename;
    ofstream fout(filename);
    assert(!fout.fail());
    fout << mNumTestRatings << endl;
    for (vector<RatingRecordWithTime*>::size_type i = 0; i < mAllRatings.size(); ++i) {
        LOG_IF(INFO, i % 5000000 == 0) << "process" << i << "/" << mAllRatings.size();
        if (!mAllRatingsIsTestFlag[i]) continue;
        RatingRecordWithTime& rating = *mAllRatings[i];
        fout << rating.UserId() << "," << rating.ItemId() << ","
             << rating.Rating() << "," << rating.Timestamp() << endl;
    }
}

void NetflixPreparation::FreeAllRatings() {
    for (RatingRecordWithTime* pRating : mAllRatings) {
        delete pRating;
    }
}

int NetflixPreparation::GetItemIdFromFileName(const std::string& filename) {
    auto begPos = filename.find('_') + 1;
    auto endPos = filename.find('.');
    return Integer::ParseInt(filename.substr(begPos, endPos - begPos));
}

int NetflixPreparation::GetTimestampFromDate(const std::string& date) {
    // date has the format YYYY-MM-DD.
    return Integer::ParseInt(date.substr(0, 4) + date.substr(5, 2) + date.substr(8, 2));
}

} //~ namespace longan
