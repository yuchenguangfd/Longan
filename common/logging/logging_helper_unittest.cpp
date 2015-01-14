/*
 * logging_helper_unittest.cpp
 * Created on: Aug 31, 2014
 * Author: chenguangyu
 */

#include "logging_helper.h"

using namespace longan;

void Poo() {
    LOG_ENTER_FUNC("test0");
    LOG_LEAVE_FUNC("test1");
}
void Bar() {
    LOG_FUNC("test2");
    LOG(INFO) << "do something in Bar()";
}

void Foo() {
    LOG_FUNC("test3");
    LOG(INFO) << "do something in Foo()";
    Bar();
}

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = true;
    Poo();
    Foo();
    return 0;
}



