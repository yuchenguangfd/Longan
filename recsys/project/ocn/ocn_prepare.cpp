/*
 * ocn_prepare.cpp
 * Created on: Feb 26, 2015
 * Author: chenguangyu
 */

#include "ocn_prepare.h"
#include "common/common.h"

namespace longan {

OcnPrepare::OcnPrepare(const std::string& inputDirpath, const std::string& configFilepath, const std::string& outputDirpath) :
    mInputDirpath(inputDirpath), mConfigFilepath(configFilepath), mOutputDirpath(outputDirpath),
    mNumUser(0), mNumItem(0), mNumRating(0) { }

void OcnPrepare::Prepare() {
    Log::I("recsys", "OcnPrepare::Prepare()");
    LoadConfig();
    ReadRatingData();
    WriteRatingData();
    Cleanup();
}

void OcnPrepare::LoadConfig() {
    Log::I("recsys", "OcnPrepare::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    JsonConfigHelper::LoadFromFile(mConfigFilepath, mConfig);
}

void OcnPrepare::ReadRatingData() {
    Log::I("recsys", "OcnPrepare::ReadRatingData()");
    std::string sessionDir = mInputDirpath + "/session";
    FileLister lister(sessionDir);
    std::vector<std::string> sessionFiles = lister.ListFilename();
    const std::string sessionFileFrom = mConfig["dateFrom"].asString() + ".txt";
    const std::string sessionFileTo = mConfig["dateTo"].asString() + ".txt";
    int watchTimeThreshold = mConfig["watchTimeThreshold"].asInt();
    mRatings.clear();
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
                if (session.length < watchTimeThreshold) continue;
                int userId, itemId;
                if (mUserIdMapping.find(session.userMac) == mUserIdMapping.end()) {
                    mUserIdMapping[session.userMac] = userCount;
                    userId = userCount;
                    ++userCount;
                } else {
                    userId = mUserIdMapping[session.userMac];
                }
                if (mItemIdMapping.find(session.assetName) == mItemIdMapping.end()) {
                    mItemIdMapping[session.assetName] = itemCount;
                    itemId = itemCount;
                    ++itemCount;
                } else {
                    itemId = mItemIdMapping[session.assetName];
                }
                float rating = (float)session.length;
                int64 timestamp = session.timestamp;
                mRatings.push_back(new RatingRecordWithTime(userId, itemId, rating, timestamp));
            }
        }
    }
    mNumRating = mRatings.size();
    mNumUser = userCount;
    mNumItem = itemCount;
    WriteUserIdMapping();
    WriteItemIdMapping();
}

void OcnPrepare::ReadSessionFile(const std::string& filename) {
    Log::Console("recsys", "loading session file %s...", filename.c_str());
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

void OcnPrepare::WriteUserIdMapping() {
    std::string filename = mOutputDirpath + "/user_id_mapping.txt";
    std::ofstream fout(filename.c_str());
    assert(!fout.fail());
    fout << mNumUser << std::endl;
    for (auto it = mUserIdMapping.begin(); it != mUserIdMapping.end(); ++it) {
        fout << it->first << "," << it->second << std::endl;
    }
}

void OcnPrepare::WriteItemIdMapping() {
    std::string filename = mOutputDirpath + "/item_id_mapping.txt";
    std::ofstream fout(filename.c_str());
    assert(!fout.fail());
    fout << mNumItem << std::endl;
    for (auto it = mItemIdMapping.begin(); it != mItemIdMapping.end(); ++it) {
        fout << it->first << "," << it->second << std::endl;
    }
}

void OcnPrepare::WriteRatingData() {
    Log::I("recsys", "OcnPrepare::WriteRatingData()");
    std::string filenameRatingData = mOutputDirpath + "/rating.txt";
    Log::I("recsys", "writing ratings to file = " + filenameRatingData);
    std::ofstream fout(filenameRatingData.c_str());
    assert(!fout.fail());
    fout << mNumRating << "," << mNumUser << "," << mNumItem << std::endl;
    for (int i = 0; i < mNumRating; ++i) {
        RatingRecordWithTime& rating = *mRatings[i];
        fout << rating.UserId() << "," << rating.ItemId() << ","
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

void OcnPrepare::GenerateRatingDataBySplitAll() {
    ArrayHelper::RandomShuffle(mRatings.data(), mRatings.size());
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
        fout1 << rating.UserId() << "," << rating.ItemId() << ","
              << rating.Rating() << "," << rating.Timestamp() << std::endl;
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
    int numTestRatings = mRatings.size() - splitPos;
    fout2 << numTestRatings << "," << mNumUser << "," << mNumItem << std::endl;
    for (int i = splitPos, j = 0; i < mRatings.size(); ++i, ++j) {
        const RatingRecordWithTime& rating = *mRatings[i];
        fout2 << rating.UserId() << "," << rating.ItemId() << ","
             << rating.Rating() << "," << rating.Timestamp() << std::endl;
        if (j % 500000 == 0) Log::Console("recsys", "%d/%d done.", j, numTestRatings);
    }
}

void OcnPrepare::GenerateRatingDataBySplitUser() {
    Log::I("recsys", "OcnPrepare::GenerateRatingDataBySplitUser()");
    mSort(mRatings.data(), mRatings.size(),
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
        fout1 << rating.UserId() << ","
              << rating.ItemId() << ","
              << rating.Rating() << "," << rating.Timestamp() << std::endl;
    }

    std::string filenameTest = mOutputDirpath + "/rating_test.txt";
    Log::I("recsys", "writing test ratings to file: " + filenameTest);
    std::ofstream fout2(filenameTest.c_str());
    assert(!fout2.fail());
    fout2 << testRatings.size() << "," << mNumUser << "," << mNumItem << std::endl;
    for (int i = 0; i < testRatings.size(); ++i) {
        RatingRecordWithTime& rating = *testRatings[i];
        fout2 << rating.UserId() << ","
              << rating.ItemId() << ","
              << rating.Rating() << "," << rating.Timestamp() << std::endl;
    }
}

void OcnPrepare::Cleanup() {
    Log::I("recsys", "OcnPrepare::Cleanup()");
    for (RatingRecordWithTime *pRating: mRatings) {
        delete pRating;
    }
}

} //~ namespace longan
