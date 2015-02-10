/*
 * triangle.h
 * Created on: Feb 6, 2015
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GEOMETRY_TRIANGLE_H
#define ALGORITHM_GEOMETRY_TRIANGLE_H

#include "point.h"

namespace longan {

template <class T>
class Triangle {
public:
    Triangle(const Point2D<T> &p0, const Point2D<T> &p1, const Point2D<T> &p2) :
        mP0(p0), mP1(p1), mP2(p2) { }
    Point2D64F ComputeMedianPoint() const;
    Point2D64F ComputeOrthocenter() const;
    Point2D64F ComputeCircumcenter() const;
private:
    Point2D<T> mP0;
    Point2D<T> mP1;
    Point2D<T> mP2;
};

typedef Triangle<double> Triangle64F;

} //~ namespace longan


#endif /* ALGORITHM_GEOMETRY_TRIANGLE_H */
