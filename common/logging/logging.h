/*
 * logging.h
 * Created on: Oct 30, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_LOGGING_LOGGING_H
#define COMMON_LOGGING_LOGGING_H

#include "common/lang/defines.h"
#include <glog/logging.h>
#include <string>
#include <cstdio>

namespace longan {

class Log {
public:
    inline static void I(const std::string& tag, const char *msg) {
        LOG(INFO) << '[' << tag << ']' << msg;
    }

    inline static void I(const std::string& tag, const std::string& msg) {
        Log::I(tag, msg.c_str());
    }

    template <class ...T>
    inline static void I(const std::string& tag, const char *format, T... args) {
        char buffer[MESSAGE_BUFFER_SIZE];
        sprintf(buffer, format, args...);
        Log::I(tag, buffer);
    }

    inline static void W(const std::string& tag, const char *msg) {
        LOG(WARNING) << '[' << tag << ']' << msg;
    }

    inline static void W(const std::string& tag, const std::string& msg) {
        Log::W(tag, msg.c_str());
    }

    template <class ...T>
    inline static void W(const std::string& tag, const char *format, T... args) {
        char buffer[MESSAGE_BUFFER_SIZE];
        sprintf(buffer, format, args...);
        Log::W(tag, buffer);
    }

    inline static void E(const std::string& tag, const char *msg) {
        LOG(ERROR) << '[' << tag << ']' << msg;
    }

    inline static void E(const std::string& tag, const std::string& msg) {
        Log::E(tag, msg.c_str());
    }

    template <class ...T>
    inline static void E(const std::string& tag, const char *format, T... args) {
        char buffer[MESSAGE_BUFFER_SIZE];
        sprintf(buffer, format, args...);
        Log::E(tag, buffer);
    }

    inline static void Console(const std::string& tag, const char *msg) {
        fprintf(stderr, "[%s]%s\r", tag.c_str(), msg);
    }

    inline static void Console(const std::string& tag, const std::string& msg) {
        fprintf(stderr, "[%s]%s\r", tag.c_str(), msg.c_str());
    }

    template <class ...T>
    inline static void Console(const std::string& tag, const char *format, T... args) {
        fprintf(stderr, "[%s]", tag.c_str());
        fprintf(stderr, format, args...);
        fprintf(stderr, "\r");
    }
private:
    static const int MESSAGE_BUFFER_SIZE = 4096;
    DISALLOW_IMPLICIT_CONSTRUCTORS(Log);
};

} //~ namespace longan

#endif /* COMMON_LOGGING_LOGGING_H */
