/*
 * segment.h
 * Created on: Feb 3, 2015
 * Author: yuchenguang
 */

#ifndef ALGORITHM_GEOMETRY_SEGMENT_H
#define ALGORITHM_GEOMETRY_SEGMENT_H

#include "point.h"

namespace longan {

template <class T>
class Segment2D {
public:
private:
	Point2D<T> mStart;
	Point2D<T> mEnd;
};

} //~ namespace longan

#endif /* ALGORITHM_GEOMETRY_SEGMENT_H */
