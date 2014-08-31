/*
 * logging_helper.h
 * Created on: Aug 31, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_LOGGING_LOGGING_HELPER_H
#define COMMON_LOGGING_LOGGING_HELPER_H

#include <glog/logging.h>

namespace longan {

#define LOG_ENTER_FUNC  LOG(INFO) << "enter func " << __func__
#define LOG_LEAVE_FUNC  LOG(INFO) << "leave func " << __func__

}

#endif /* COMMON_LOGGING_LOGGING_HELPER_H */
