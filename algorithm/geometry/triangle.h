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
    double ComputeArea() const;
    Point2D64F ComputeMedianPoint() const;
    Point2D64F ComputeOrthocenter() const;
    Point2D64F ComputeCircumcenter() const;
private:
    Point2D<T> mP0;
    Point2D<T> mP1;
    Point2D<T> mP2;
};

template <>
double Triangle<double>::ComputeArea() const {
    return fabs(CrossProduct(mP1 - mP0, mP2 - mP0) / 2.0);
}

template <>
Point2D64F Triangle<double>::ComputeCircumcenter() const {
    double ax = mP0.x, ay = mP0.y;
    double bx = mP1.x, by = mP1.y;
    double cx = mP2.x, cy = mP2.y;
    double c1 = (ax*ax+ay*ay-bx*bx-by*by)/2;
    double c2 = (ax*ax+ay*ay-cx*cx-cy*cy)/2;
    Point2D64F center;
    center.x = (c1*(ay-cy)-c2*(ay-by))/((ax-bx)*(ay-cy)-(ax-cx)*(ay-by));
    center.y = (c1*(ax-cx)-c2*(ax-bx))/((ay-by)*(ax-cx)-(ay-cy)*(ax-bx));
    return center;
}

typedef Triangle<double> Triangle64F;

} //~ namespace longan


#endif /* ALGORITHM_GEOMETRY_TRIANGLE_H */
