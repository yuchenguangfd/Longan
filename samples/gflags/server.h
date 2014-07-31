#include <gflags/gflags.h>
#include <iostream>

DECLARE_int32(http_port);
DECLARE_int32(https_port);

class Server {
public:
    Server() {
        std::cout << "HTTP listen port: " << FLAGS_http_port << std::endl;
        std::cout << "HTTPS listen port: " << FLAGS_https_port << std::endl;
    }
};
