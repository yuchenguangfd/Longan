/*
 * logging.h
 * Created on: Oct 30, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_LOGGING_LOGGING_H
#define COMMON_LOGGING_LOGGING_H

#include <glog/logging.h>
#include <string>
#include <cstdio>

namespace longan {

class Log {
public:
    inline static void I(const std::string& tag, const std::string& msg) {
        LOG(INFO) << '[' << tag << ']'
                  << msg;
    }

    inline static void W(const std::string& tag, const std::string& msg) {
        LOG(WARNING) << '[' << tag << ']'
                     << msg;
    }

    inline static void E(const std::string& tag, const std::string& msg) {
        LOG(ERROR) << '[' << tag << ']'
                   << msg;
    }

};

class ConsoleLog {
public:
    inline static void I(const std::string& tag, const std::string& msg) {
        fprintf(stderr, "[%s]%s\n", tag.c_str(), msg.c_str());
    }
};

} //~ namespace longan

#endif /* COMMON_LOGGING_LOGGING_H */
