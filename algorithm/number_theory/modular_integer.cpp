/*
 * modular_integer.cpp
 * Created on: Apr 25, 2015
 * Author: chenguangyu
 */

#include "modular_integer.h"

namespace longan {

int ModularInteger::sModular = ModularInteger::DEFAULT_MODULAR;

void ModularInteger::SetModular(int modular) {
	sModular = modular;
}

} //~ namespace longan
