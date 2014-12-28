/*
 * basic_convert.h
 * Created on: Dec 11, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_BASIC_CONVERT_H
#define RECSYS_BASE_BASIC_CONVERT_H

#include <string>

namespace longan {

class BasicConvert {
public:
    BasicConvert(const std::string& ratingTextFilepath, const std::string& ratingBinaryFilepath);
    virtual ~BasicConvert();
    virtual void Convert();
protected:
    const std::string& mRatingTextFilepath;
    const std::string& mRatingBinaryFilepath;
};

} //~ namespace longan

#endif /* RECSYS_BASE_BASIC_CONVERT_H */
