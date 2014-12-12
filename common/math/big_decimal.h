/*
 * big_decimal.h
 * Created on: Dec 9, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MATH_BIG_DECIMAL_H
#define COMMON_MATH_BIG_DECIMAL_H

#include <string>
#include <vector>
#include <iostream>

namespace longan {

class BigDecimal {
public:
    BigDecimal(const std::string& decimal);
    ~BigDecimal();
    BigDecimal& operator*= (const BigDecimal& rhs);
    std::string ToString() const;
private:
    static const int BASE = 10;
    int mNumDigits;
    int mPointPosition;
    std::vector<int> mDigits;
};

} //~ namespace longan

#endif /* COMMON_MATH_BIG_DECIMAL_H */
