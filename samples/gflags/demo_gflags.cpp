/*
 * demo_gflags.cpp
 * Created on: Jul 31, 2014
 * Author: chenguangyu
 */

#include "server.h"
#include <gflags/gflags.h>
#include <string>
#include <iostream>

using namespace std;

DEFINE_bool(memory_pool, false, "If use memory pool");
DEFINE_bool(daemon, true, "If started as daemon");
DEFINE_string(module_id, "4123", "Server module id");
DEFINE_int32(http_port, 80, "HTTP listen port");
DEFINE_int32(https_port, 443, "HTTPS listen port");

int main(int argc, char** argv) {
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    cout << "Server module id: " << FLAGS_module_id << endl;
    if (FLAGS_daemon) {
        cout << "Run as daemon: " << FLAGS_daemon << endl;
    }
    if (FLAGS_memory_pool) {
        cout << "Use memory pool: " << FLAGS_daemon << endl;
    }
    Server server;
    return 0;
}
