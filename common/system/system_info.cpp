/*
 * system_info.cpp
 * Created on: Nov 4, 2014
 * Author: chenguangyu
 */

#include "system_info.h"
#include <thread>

namespace longan {

int SystemInfo::GetNumCPUCore() {
    int numCore = std::thread::hardware_concurrency();
    if (numCore > 0) {
        return numCore;
    } else {
        return 1;
    }
}

} //~ namespace longan
