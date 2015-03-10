/*
 * implict_convert.h
 * Created on: Feb 27, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_UTIL_IMPLICT_CONVERT_H
#define RECSYS_UTIL_IMPLICT_CONVERT_H

#include <string>

namespace longan {

class ImplictConvert {
public:
    ImplictConvert(const std::string& explictRatingTextFilepath, double negativeToPositiveRatio,
            const std::string& implictRatingTextFilepath);
    void Convert();
private:
    const std::string mExplictRatingTextFilepath;
    const double mNegativeToPositiveRatio;
    const std::string mImplictRatingTextFilepath;

};

} //~ namespace longan

#endif /* RECSYS_UTIL_IMPLICT_CONVERT_H */
