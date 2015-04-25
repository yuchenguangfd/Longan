/*
 * fast_power.h
 * Created on: Apr 25, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_NUMBER_THEORY_FAST_POWER_H_
#define ALGORITHM_NUMBER_THEORY_FAST_POWER_H_

namespace longan {

template <class T>
T FastPower(const T& x, int n) const {
	T result = x;
	--n;
	T p = x;
	while (n != 0) {
		if (n & 0x1) {
			result = result * p;
		}
		p = p * p;
		n >>= 1;
	}
	return result;
}

} //~ namespace longan

#endif /* ALGORITHM_NUMBER_THEORY_FAST_POWER_H_ */
