/*
 * big_decimal.cpp
 * Created on: Dec 9, 2014
 * Author: chenguangyu
 */

#include "big_decimal.h"
#include "common/lang/character.h"
#include "common/lang/integer.h"

namespace longan {

BigDecimal::BigDecimal(const std::string& decimal) :
    mNumDigits(0),
    mPointPosition(0) {
    mDigits.reserve(decimal.size());
    for(int i = decimal.size() - 1; i >= 0; --i) {
        if(Character::IsDigit(decimal[i])) {
            ++mNumDigits;
            mDigits.push_back(Character::ToDigit(decimal[i]));
        } else if(decimal[i] == '.') {
            mPointPosition = mNumDigits;
        }
    }
}

BigDecimal::~BigDecimal() { }

BigDecimal& BigDecimal::operator*= (const BigDecimal& rhs) {
    std::vector<int> prod(mNumDigits + rhs.mNumDigits);
    int L = mNumDigits + rhs.mNumDigits - 1;
    for(int i = 0; i < mNumDigits; ++i) {
       for(int j = 0; j < rhs.mNumDigits; ++j) {
          prod[i + j] += mDigits[i] * rhs.mDigits[j];
       }
    }
    int carry = 0;
    for(int i = 0; i < prod.size(); ++i) {
        prod[i] += carry;
        carry = prod[i] / BASE;
        prod[i] %= BASE;
    }
    if (prod[prod.size()-1] > 0) {
        mNumDigits = prod.size();
    } else {
        mNumDigits = prod.size() - 1;
    }
    mPointPosition += rhs.mPointPosition;
    mDigits = std::move(prod);
    return *this;
}

std::string BigDecimal::ToString() const {
    std::string result;
    int low = 1;
    while(low <= mPointPosition && mDigits[low-1] == 0) {
        ++low;
    }
    int up = mNumDigits;
    if(mDigits[up-1] == 0 && mPointPosition == up-1) --up;

    if(mNumDigits > 1 && up <= low)
        result += "0";
    else
        for(int i = up; i >= low; --i) {
            if(i == mPointPosition)
                result += ".";
            result += Integer::ToString(mDigits[i-1]);
        }
    return std::move(result);
}

} //~ namespace longan
