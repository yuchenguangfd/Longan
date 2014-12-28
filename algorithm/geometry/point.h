/*
 * point.h
 * Created on: Dec 28, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GEOMETRY_POINT_H
#define ALGORITHM_GEOMETRY_POINT_H

namespace longan {

template <class T>
class Point2D {
public:
    Point2D() : x(), y() { }
    Point2D(T x, T y) { this->x = x; this->y = y;}
public:
    T x;
    T y;
};

template <class T>
inline Point2D<T> operator+ (const Point2D<T>& lhs, const Point2D<T>& rhs) {
    return Point2D<T>(lhs.x + rhs.x, lhs.x + rhs.y);
}

template <class T>
inline Point2D<T> operator- (const Point2D<T>& lhs, const Point2D<T>& rhs) {
    return Point2D<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

template <class T>
inline Point2D<T> operator* (const Point2D<T>& lhs, T scale) {
    return Point2D<T>(lhs.x * scale, lhs.y * scale);
}

template <class T>
inline Point2D<T> operator* (T scale, const Point2D<T>& rhs) {
    return Point2D<T>(scale * rhs.x, scale * rhs.y);
}

template <class T>
inline Point2D<T> operator/ (const Point2D<T>& lhs, T scale) {
    return Point2D<T>(lhs.x / scale, lhs.y / scale);
}
//
//T dotProduct(const Point2D<T> &p1) const;
//    T crossProduct(const Point2D<T> &p1) const;
//    double length() const;
//    double distance(const Point2D<T> &p1) const;
//
//template <class T>
//inline T Point2D<T>::dotProduct(const Point2D<T> &p1) const {
//    T result = x * p1.x + y * p1.y;
//    return result;
//}

template <class T>
inline T CrossProduct(const Point2D<T>& lhs, const Point2D<T>& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}
//
//template <class T>
//inline double Point2D<T>::length() const {
//    double result = sqrt(x * x + y * y);
//    return result;
//}
//
//template <class T>
//inline double Point2D<T>::distance(const Point2D<T> &p1) const {
//    double dx = x - p1.x;
//    double dy = y - p1.y;
//    double result = sqrt(dx * dx + dy * dy);
//    return result;
//}

typedef Point2D<int> Point2D32I;
typedef Point2D<double>	Point2D64F;

}  //~ namespace longan

#endif /* ALGORITHM_GEOMETRY_POINT_H */
