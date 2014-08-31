/*
 * logging_helper_unittest.cpp
 * Created on: Aug 31, 2014
 * Author: chenguangyu
 */

#include "logging_helper.h"
#include <glog/logging.h>

using namespace longan;

void Bar() {
    LOG_ENTER_FUNC;
    LOG(INFO) << "do something in Bar()";
    LOG_LEAVE_FUNC;
}

void Foo() {
    LOG_ENTER_FUNC;
    LOG(INFO) << "do something in Foo()";
    Bar();
    LOG_LEAVE_FUNC;
}

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = true;
    Bar();
    Foo();
    return 0;
}



