/*
 * implict_convert.cpp
 * Created on: Feb 27, 2015
 * Author: chenguangyu
 */

#include "implict_convert.h"
#include "recsys/base/rating_list.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "common/logging/logging.h"
#include "common/lang/text_output_stream.h"
#include "common/util/random.h"
#include "common/math/math.h"

namespace longan {

ImplictConvert::ImplictConvert(const std::string& explictRatingTextFilepath, int negativeToPositiveRatio,
        const std::string& implictRatingTextFilepath) :
    mExplictRatingTextFilepath(explictRatingTextFilepath),
    mNegativeToPositiveRatio(negativeToPositiveRatio),
    mImplictRatingTextFilepath(implictRatingTextFilepath) { }

void ImplictConvert::Convert() {
    Log::I("recsys", "ImplictConvert::Convert()");
    Log::I("recsys", "load explict rating text file = " + mExplictRatingTextFilepath);
    RatingList posRatings = RatingList::LoadFromTextFile(mExplictRatingTextFilepath);
    std::vector<RatingRecord*> negRatings;
    if (mNegativeToPositiveRatio > 0) {
        Log::I("recsys", "sampling negative rating(ratio=%d) ...", mNegativeToPositiveRatio);
        RatingMatUsers mat;
        mat.Init(posRatings);
        std::vector<bool> itemsAvailable;
        for (int uid = 0; uid < mat.NumUser(); ++uid) {
            itemsAvailable.assign(mat.NumItem(), true);
            const UserVec& uv = mat.GetUserVector(uid);
            const ItemRating* data = uv.Data();
            int size = uv.Size();
            for (int i = 0; i < size; ++i) {
                itemsAvailable[data[i].ItemId()] = false;
            }
            int numNeg = Math::Min(mNegativeToPositiveRatio * size, mat.NumItem()-size);
            int countNeg = 0;
            int limit = 0, maxLimit = 3*numNeg;
            while ((++limit) < maxLimit) {
                int iid = Random::Instance().Uniform(0, mat.NumItem());
                if (!itemsAvailable[iid]) continue;
                itemsAvailable[iid] = false;
                negRatings.push_back(new RatingRecord(uid, iid, 0.0f));
                ++countNeg;
                if (countNeg >= numNeg) break;
            }
        }
        Log::I("recsys", "sampling done. num positive rating = %d, num negative rating = %d",
                posRatings.NumRating(), negRatings.size());
    }
    Log::I("recsys", "write implict rating text file = " + mImplictRatingTextFilepath);
    TextOutputStream tos(mImplictRatingTextFilepath);
    int numRating = posRatings.NumRating() + negRatings.size();
    tos << numRating << ',' << posRatings.NumUser() << ',' << posRatings.NumItem() << '\n';
    for (int i = 0; i < posRatings.NumRating(); ++i) {
        RatingRecord& rr = posRatings[i];
        tos << rr.UserId() << ',' << rr.ItemId() << ','
            << '1' << ',' << 0LL <<'\n';
    }
    for (int i = 0; i < negRatings.size(); ++i) {
        RatingRecord *rr = negRatings[i];
        tos << rr->UserId() << ',' << rr->ItemId() << ','
            << '0' << ',' << 0LL << '\n';
    }
    for (RatingRecord* rr : negRatings) {
        delete rr;
    }
}

} //~ namespace longan
