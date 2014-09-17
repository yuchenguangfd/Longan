/*
 * complex.h
 * Created on: Sep 13, 2014
 * Author: chenguangyu
 */

#include "math.h"

namespace longan {

template <class T>
class Complex {
public:
    Complex(T real, T imag) : mReal(real), mImag(imag) {}
    T Real() const { return mReal; }
    T Imag() const { return mImag; }
    T Radius() const { return Math::Sqrt(mReal*mReal + mImag*mImag); }
private:
    T mReal;
    T mImag;
};

typedef Complex<int> Complex32I;
typedef Complex<float> Complex32F;
typedef Complex<double> Complex64F;

} //~ namespace longan
