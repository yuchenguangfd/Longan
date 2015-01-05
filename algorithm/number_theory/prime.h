/*
 * prime.h
 * Created on: Jan 4, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_NUMBER_THEORY_PRIME_H
#define ALGORITHM_NUMBER_THEORY_PRIME_H

#include <vector>

namespace longan {

class PrimeFactorizationResult {
public:
    int Size() const {
        return mFactors.size();
    }
    int Factor(int i) const {
        return mFactors[i];
    }
    int Power(int i) const {
        return mPowers[i];
    }
    void Add(int factor, int power) {
        mFactors.push_back(factor);
        mPowers.push_back(power);
    }
    void Clear() {
        mFactors.clear();
        mPowers.clear();
    }
private:
    std::vector<int> mFactors;
    std::vector<int> mPowers;
};

void PrimeFactorization(int N, PrimeFactorizationResult *result);

} //~ namespace longan

#endif /* ALGORITHM_NUMBER_THEORY_PRIME_H */
