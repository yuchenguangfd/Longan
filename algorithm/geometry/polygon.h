/*
 * polygon.h
 * Created on: Dec 28, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_GEOMETRY_POLYGON_H
#define ALGORITHM_GEOMETRY_POLYGON_H

#include "point.h"
#include "common/math/math.h"
#include <vector>

namespace longan {

template <class T>
class Polygon {
public:
    Polygon() { }
    void AddVertex(const Point2D<T>& p) {
        mVertices.push_back(p);
    }
    T ComputeArea() const;
private:
    std::vector<Point2D<T>> mVertices;
};

template <class T>
T Polygon<T>::ComputeArea() const {
    T area = T();
    for(int i = 1; i < mVertices.size(); ++i) {
        area +=  CrossProduct(mVertices[i-1], mVertices[i]);
    }
    area += CrossProduct(mVertices[mVertices.size()-1], mVertices[0]);
    area /= 2;
    return Math::Abs(area);
}
////    public Point getCenterOfMass()
////    {
////        int n = numberOfVertex;
////        double area = 0.0, cross;
////        Point com(0.0, 0.0);
////        for(int i = 1; i < n; ++i){
////            cross = vertex[i-1].x*vertex[i].y - vertex[i-1].y*vertex[i].x;
////            com.x += cross*(vertex[i-1].x+vertex[i].x)/6;
////            com.y += cross*(vertex[i-1].y+vertex[i].y)/6;
////            area += cross/2;
////        }
////        cross = vertex[n-1].x*vertex[0].y - vertex[n-1].y*vertex[0].x;
////        com.x += cross*(vertex[n-1].x+vertex[0].x)/6;
////        com.y += cross*(vertex[n-1].y+vertex[0].y)/6;
////        area += cross/2;
////        com.x /= area;
////        com.y /= area;
////        return com;
////    }

} //~ namespace longan

#endif /* ALGORITHM_GEOMETRY_POLYGON_H */
