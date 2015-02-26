/*
 * ocn_prepare.cpp
 * Created on: Feb 26, 2015
 * Author: chenguangyu
 */

#include "ocn_prepare.h"
#include "common/config/json_config_helper.h"
#include "common/logging/logging.h"
#include "common/system/file_lister.h"
#include "common/lang/integer.h"
#include "common/util/string_helper.h"
#include "common/util/array_helper.h"
#include <algorithm>
#include <fstream>
#include <cassert>

namespace longan {

OcnPrepare::OcnPrepare(const std::string& inputDirpath, const std::string& configFilepath, const std::string& outputDirpath) :
    mInputDirpath(inputDirpath), mConfigFilepath(configFilepath), mOutputDirpath(outputDirpath),
    mNumUser(0), mNumItem(0), mNumRating(0) { }

void OcnPrepare::Prepare() {
    Log::I("recsys", "OcnPrepare::Prepare()");
    LoadConfig();
    ReadVideoInfo();
    GenerateUserAndItemIdMapping();
    GenerateMovieData();
    GenerateRatingData();
    Cleanup();
}

void OcnPrepare::LoadConfig() {
    Log::I("recsys", "OcnPrepare::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    JsonConfigHelper::LoadFromFile(mConfigFilepath, mConfig);
}

void OcnPrepare::ReadVideoInfo() {
    Log::I("recsys", "OcnPrepare::ReadVideoInfo()");
    std::ifstream fin;
    std::string itemInfoFile = mInputDirpath + "/video/video.txt";
    fin.open(itemInfoFile.c_str());
    assert(!fin.fail());
    mVideos.clear();
    std::string line;
    while (std::getline(fin, line)) {
        std::vector<std::string> lineFields = StringHelper::Split(line, "|", false);
        Video video;
        video.assetName = lineFields[0];
        video.duration = Integer::ParseInt(lineFields[1]);
        video.title = lineFields[2];
        mVideos.push_back(video);
    }
    mItemDurationMapping.clear();
    for (const Video& video : mVideos) {
        mItemDurationMapping[video.assetName] = video.duration;
    }
}

void OcnPrepare::GenerateUserAndItemIdMapping() {
    Log::I("recsys", "OcnPrepare::GenerateUserAndItemIdMapping()");
    std::string sessionDir = mInputDirpath + "/session";
    FileLister lister(sessionDir);
    std::vector<std::string> sessionFiles = lister.ListFilename();
    const std::string sessionFileFrom = mConfig["dateFrom"].asString() + ".txt";
    const std::string sessionFileTo = mConfig["dateTo"].asString() + ".txt";
    mUserIdMapping.clear();
    mItemIdMapping.clear();
    int userCount = 0;
    int itemCount = 0;
    for (int i = 0; i < sessionFiles.size(); ++i) {
        const std::string& filename = sessionFiles[i];
        if (sessionFileFrom <= filename && filename <= sessionFileTo) {
            mSessions.clear();
            mSessions.shrink_to_fit();
            ReadSessionFile(filename);
            for (const Session session : mSessions) {
                if (mUserIdMapping.find(session.userMac) == mUserIdMapping.end()) {
                    mUserIdMapping[session.userMac] = userCount;
                    ++userCount;
                }
                if (mItemDurationMapping.find(session.assetName) != mItemDurationMapping.end()
                        && mItemIdMapping.find(session.assetName) == mItemIdMapping.end()) {
                    mItemIdMapping[session.assetName] = itemCount;
                    ++itemCount;
                }
            }
        }
    }

    mNumUser = userCount;
    std::string filename1 = mOutputDirpath + "/user_id_mapping.txt";
    std::ofstream fout1(filename1.c_str());
    assert(!fout1.fail());
    fout1 << mNumUser << std::endl;
    for (auto it = mUserIdMapping.begin(); it != mUserIdMapping.end(); ++it) {
        fout1 << it->first << "," << it->second << std::endl;
    }

    mNumItem = itemCount;
    std::string filename2 = mOutputDirpath + "/item_id_mapping.txt";
    std::ofstream fout2(filename2.c_str());
    assert(!fout2.fail());
    fout2 << mNumItem << std::endl;
    for (auto it = mItemIdMapping.begin(); it != mItemIdMapping.end(); ++it) {
       fout2 << it->first << "," << it->second << std::endl;
    }
}

void OcnPrepare::ReadSessionFile(const std::string& filename) {
    Log::I("recsys", "loading session file %s...", filename.c_str());
    std::string sessionFile = mInputDirpath + "/session/" + filename;
    std::ifstream fin(sessionFile.c_str());
    assert(!fin.fail());
    std::string line;
    while (std::getline(fin, line)) {
        std::vector<std::string> lineFields = StringHelper::Split(line, "|");
        Session session;
        session.userMac = lineFields[0];
        session.assetName = lineFields[1];
        session.length = Integer::ParseInt(lineFields[2]);
        session.timestamp = GetTimestamp(lineFields[3]);
        mSessions.push_back(session);
    }
}

int64 OcnPrepare::GetTimestamp(const std::string& time) {
    // time has the format like 2013-01-03 23:41:51
    return Integer::ParseInt64(time.substr(0, 4) + time.substr(5, 2) + time.substr(8, 2)
                + time.substr(11, 2) + time.substr(14, 2) + time.substr(17, 2));
}

void OcnPrepare::GenerateMovieData() {
    Log::I("recsys", "OcnPrepare::GenerateMovieData()");
    std::string filename = mOutputDirpath + "/movie.txt";
    std::ofstream fout(filename.c_str());
    assert(!fout.fail());
    int countValidMovie = 0;
    for (const Video& video: mVideos) {
        if (mItemIdMapping.find(video.assetName) != mItemIdMapping.end()) {
            ++countValidMovie;
        }
    }
    fout << countValidMovie << std::endl;
    for (const Video& video: mVideos) {
        if (mItemIdMapping.find(video.assetName) != mItemIdMapping.end()) {
            fout << mItemIdMapping[video.assetName] << ","
                 << video.duration << ","
                 << video.title << std::endl;
        }
    }
}

void OcnPrepare::GenerateRatingData() {
    Log::I("recsys", "OcnPrepare::GenerateRatingData()");
    std::string sessionDir = mInputDirpath + "/session";
    FileLister lister(sessionDir);
    std::vector<std::string> sessionFiles = lister.ListFilename();
    const std::string sessionFileFrom = mConfig["dateFrom"].asString() + ".txt";
    const std::string sessionFileTo = mConfig["dateTo"].asString() + ".txt";
    mRatings.clear();
    for (int i = 0; i < sessionFiles.size(); ++i) {
        const std::string& filename = sessionFiles[i];
        if (sessionFileFrom <= filename && filename <= sessionFileTo) {
            mSessions.clear();
            mSessions.shrink_to_fit();
            ReadSessionFile(filename);
            for (const Session session : mSessions) {
                if (mItemIdMapping.find(session.assetName) == mItemIdMapping.end()) continue;
                int userId = mUserIdMapping[session.userMac];
                int itemId = mItemIdMapping[session.assetName];
                float rating = mSessionLengthToRatingFunc(session.length,
                        mItemDurationMapping[session.assetName]);
                int64 timestamp = session.timestamp;
                mRatings.push_back(new RatingRecordWithTime(userId, itemId, rating, timestamp));
            }
        }
    }
    mNumRating = mRatings.size();

    ArrayHelper::RandomShuffle(&mRatings[0], mRatings.size());
    double trainRatio = mConfig["trainRatio"].asDouble();
    int splitPos = static_cast<int>(mNumRating * trainRatio);

    std::string filenameTrain = mOutputDirpath + "/rating_train.txt";
    Log::I("recsys", "writing train ratings to file = " + filenameTrain);
    std::ofstream fout1(filenameTrain.c_str());
    assert(!fout1.fail());
    mSort(mRatings.data(), splitPos,
        [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs)->int {
            return lhs->Timestamp() < rhs->Timestamp() ? -1 : 1;
    });
    int numTrainRatings = splitPos;
    fout1 << numTrainRatings << ","
          << mNumUser << ","
          << mNumItem << std::endl;
    for (int i = 0; i < splitPos; ++i) {
        const RatingRecordWithTime& rating = *mRatings[i];
        fout1 << rating.UserId() << ","
              << rating.ItemId() << ","
              << rating.Rating() << ","
              << rating.Timestamp() << std::endl;
        if (i % 500000 == 0) Log::Console("recsys", "%d/%d done.", i, numTrainRatings);
    }

    std::string filenameTest = mOutputDirpath + "/rating_test.txt";
    Log::I("recsys", "writing test ratings to file: " + filenameTest);
    std::ofstream fout2(filenameTest.c_str());
    assert(!fout2.fail());
    mSort(mRatings.data() + splitPos, mRatings.size() - splitPos,
        [](const RatingRecordWithTime *lhs, const RatingRecordWithTime *rhs)->int {
            return lhs->Timestamp() < rhs->Timestamp() ? -1 : 1;
    });
    int numTestRatings = mRatings.size() - numTrainRatings;
    fout2 << numTestRatings << ","
          << mNumUser << ","
          << mNumItem << std::endl;
    for (int i = splitPos; i < mRatings.size(); ++i) {
        const RatingRecordWithTime& rating = *mRatings[i];
        fout2 << rating.UserId() << ","
              << rating.ItemId() << ","
              << rating.Rating() << ","
              << rating.Timestamp() << std::endl;
    }
}

void OcnPrepare::Cleanup() {
    Log::I("recsys", "OcnPrepare::Cleanup()");
    for (RatingRecordWithTime *pRating: mRatings) {
        delete pRating;
    }
}

} //~ namespace longan
