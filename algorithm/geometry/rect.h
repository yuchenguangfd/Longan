/*
 * rect.h
 * Created on: Jan 31, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GEOMETRY_RECT_H
#define ALGORITHM_GEOMETRY_RECT_H

namespace longan {

template <class T>
class Rect {
public:
    Rect() : mX(), mY(), mWidth(), mHeight() { };
    Rect(T x, T y, T width, T height) : mX(x), mY(y), mWidth(width), mHeight(height) { }
    T X() const { return mX; }
    T Y() const { return mY; }
    T Width() const { return mWidth; }
    T Height() const { return mHeight; }
    T Area() const { return mWidth * mHeight; }
    bool IsOverlap(const Rect<T>& arect) const {
        return mX < arect.mX + arect.mWidth
            && mX + mWidth > arect.mX
            && mY < arect.mY + arect.mHeight
            && mY + mHeight > arect.mY;
    }
private:
    T mX, mY;
    T mWidth, mHeight;
};

typedef Rect<int> Rect32I;
typedef Rect<float> Rect32F;
typedef Rect<double> Rect64F;

}  //~ namespace longan

#endif /* ALGORITHM_GEOMETRY_RECT_H */
