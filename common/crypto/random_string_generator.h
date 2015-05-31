/*
 * random_string_generator.h
 * Created on: May 31, 2015
 * Author: chenguangyu
 */

#ifndef COMMON_CRYPTO_RANDOM_STRING_GENERATOR_H
#define COMMON_CRYPTO_RANDOM_STRING_GENERATOR_H

#include "common/util/random.h"
#include "common/util/array_helper.h"
#include <string>
#include <cassert>

namespace longan {

class RandomStringGenerator {
public:
	RandomStringGenerator(int length, bool hasUpper, bool hasLower, bool hasDigit) :
		mLength(length), mHasUpper(hasUpper), mHasLower(hasLower), mHasDigit(hasDigit) { }
    std::string Generate() {
    	assert(mLength > 0);
    	assert(mHasUpper || mHasLower || mHasDigit);
		std::string randStr;
		randStr.resize(mLength);
		int type[3], ntype = 0;
		if (mHasUpper) type[ntype++] = 0;
		if (mHasLower) type[ntype++] = 1;
		if (mHasDigit) type[ntype++] = 2;
		Random& rnd = Random::Instance();
		for (int i = 0; i < mLength; ++i) {
			ArrayHelper::RandomShuffle(type, ntype);
			switch (type[0]) {
				case 0:
					randStr[i] = 'A' + rnd.Uniform(0,26);
					break;
				case 1:
					randStr[i] = 'a' + rnd.Uniform(0,26);
					break;
				case 2:
					randStr[i] = '0' + rnd.Uniform(0,10);
					break;
				default:
					break;
			}
		}
		return randStr;
    }
private:
    int mLength;
    bool mHasUpper;
    bool mHasLower;
    bool mHasDigit;
};

} //~ namespace longan

#endif /* COMMON_CRYPTO_RANDOM_STRING_GENERATOR_H */
