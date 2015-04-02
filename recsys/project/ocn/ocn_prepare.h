/*
 * ocn_prepare.h
 * Created on: Jan 22, 2015
 *  Author: chenguangyu
 */

#ifndef RECSYS_PROJECT_OCN_OCN_PREPARE_H
#define RECSYS_PROJECT_OCN_OCN_PREPARE_H

#include "recsys/base/rating_record_with_time.h"
#include "algorithm/sort/quick_sort_mt.h"
#include <json/json.h>
#include <map>
#include <string>
#include <vector>

namespace longan {

struct Session {
    std::string userMac;
    std::string assetName;
    int length;
    int64 timestamp;
};

class OcnPrepare {
public:
    OcnPrepare(const std::string& inputDirpath, const std::string& configFilepath,
            const std::string& outputDirpath);
    void Prepare();
private:
    void LoadConfig();
    void ReadRatingData();
    void ReadSessionFile(const std::string& filename);
    int64 GetTimestamp(const std::string& time);
    void WriteUserIdMapping();
    void WriteItemIdMapping();
    void WriteRatingData();
    void GenerateRatingDataBySplitAll();
    void GenerateRatingDataBySplitUser();
    void WriteTrainRatings(int splitPos);
    void WriteTestRatings(int splitPos);
    void Cleanup();

private:
    const std::string mInputDirpath;
    const std::string mConfigFilepath;
    const std::string mOutputDirpath;
    Json::Value mConfig;
    int mNumUser;
    int mNumItem;
    int mNumRating;
    std::vector<Session> mSessions;
    std::map<std::string, int> mUserIdMapping;
    std::map<std::string, int> mItemIdMapping;
    std::vector<RatingRecordWithTime*> mRatings;
    QuickSortMT mSort;
};

} //~ namespace longan

#endif /* RECSYS_PROJECT_OCN_OCN_PREPARE_H */
