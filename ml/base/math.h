/*
 * math.h
 * Created on: Jan 26, 2015
 * Author: chenguangyu
 */

#ifndef ML_BASE_MATH_H
#define ML_BASE_MATH_H

#include "common/math/math.h"
#include "common/math/vector.h"

namespace longan {

inline Vector64F Sigmoid(const Vector64F& vec){
    Vector64F result(vec.Size());
    for (int i = 0; i < vec.Size(); ++i) {
        result[i] = Math::Sigmoid(vec[i]);
    }
    return std::move(result);
}

} //~ namespace longan

#endif // ML_BASE_MATH_H
