/*
 * modular_integer.h
 * Created on: Apr 25, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_NUMBER_THEORY_MODULAR_INTEGER_H
#define ALGORITHM_NUMBER_THEORY_MODULAR_INTEGER_H

#include "common/lang/types.h"

namespace longan {

class ModularInteger {
public:
	ModularInteger(uint32 val = 0) : mValue(val % sModular) { }
	uint32 Value() const { return mValue; }
	static void SetModular(int modular);
	ModularInteger& operator+= (const ModularInteger& rhs);
	friend bool operator== (const ModularInteger& lhs, const ModularInteger& rhs);
	friend ModularInteger operator+ (const ModularInteger& lhs, const ModularInteger& rhs);
	friend ModularInteger operator* (const ModularInteger& lhs, const ModularInteger& rhs);
private:
	static const int DEFAULT_MODULAR = 0xffff;
	static int sModular;
	uint32 mValue;
};

inline bool operator== (const ModularInteger& lhs, const ModularInteger& rhs) {
	return (lhs.mValue == rhs.mValue);
}

inline ModularInteger& ModularInteger::operator+= (const ModularInteger& rhs) {
	mValue = (mValue + rhs.mValue) % sModular;
	return *this;
}

inline ModularInteger operator+ (const ModularInteger& lhs, const ModularInteger& rhs) {
	return ModularInteger(lhs.mValue + rhs.mValue);
}

inline ModularInteger operator* (const ModularInteger& lhs, const ModularInteger& rhs) {
	return ModularInteger(lhs.mValue * rhs.mValue);
}

} //~ namespace longan

#endif /* ALGORITHM_NUMBER_THEORY_MODULAR_INTEGER_H */
