/*
 * logging_helper_unittest.cpp
 * Created on: Aug 31, 2014
 * Author: chenguangyu
 */

#include "logging_helper.h"
#include <glog/logging.h>

using namespace longan;

void Bar() {
    LOG_FUNC;
    LOG(INFO) << "do something in Bar()";
}

void Foo() {
    LOG_FUNC;
    LOG(INFO) << "do something in Foo()";
    Bar();
}

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = true;
    Foo();
    return 0;
}



