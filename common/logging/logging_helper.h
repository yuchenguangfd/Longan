/*
 * logging_helper.h
 * Created on: Aug 31, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_LOGGING_LOGGING_HELPER_H
#define COMMON_LOGGING_LOGGING_HELPER_H

#include "logging.h"

namespace longan {

#define LOG_ENTER_FUNC(tag) LOG(INFO) << '[' << tag << ']' << "enter func " << __func__
#define LOG_LEAVE_FUNC(tag) LOG(INFO) << '[' << tag << ']' << "leave func " << __func__
#define LOG_FUNC(tag) FuncCallTracer trcer(tag, __func__)

class FuncCallTracer {
public:
    FuncCallTracer(const std::string& tag, const char* funcname) :
        mTag(tag), mFuncName(funcname) {
        LOG(INFO) << '[' << mTag << ']' << "entering func " << mFuncName;
    }
    ~FuncCallTracer() {
        LOG(INFO) << '[' << mTag << ']' << "leaving func " << mFuncName;
    }
private:
    const std::string mTag;
    const char* mFuncName;
};

}

#endif /* COMMON_LOGGING_LOGGING_HELPER_H */
