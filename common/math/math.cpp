/*
 * math.cpp
 * Created on: Jul 20, 2014
 * Author: chenguangyu
 */

#include "math.h"

namespace longan {

namespace Math {

int Factorial(int n) {
	int res = 1;
	for (int i = 2; i <= n; ++i)
		res *= i;
	return res;
}

}  // namespace Math

} //~ namespace longan



