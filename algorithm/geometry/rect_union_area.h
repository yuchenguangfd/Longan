/*
 * rect_union_area.h
 * Created on: Jan 31, 2015
 * Author: chenguangyu
 */

#include "rect.h"
#include "common/math/math.h"
#include <vector>

namespace longan {

template <class T>
class RectUnionArea {
public:
    T ComputeUnionArea(const std::vector<Rect<T>>& rects);
private:
    void AddRect(const Rect<T>& rect);
    void CutRect(const Rect<T>& rectCut, const Rect<T>& rectRef);
    std::vector<Rect<T>> mNonOverlapRects;
};

template <class T>
T RectUnionArea<T>::ComputeUnionArea(const std::vector<Rect<T>>& rects) {
    mNonOverlapRects.clear();
    for (int i = 0; i < rects.size(); ++i) {
        AddRect(rects[i]);
    }
    T area = T();
    for (int i = 0; i < mNonOverlapRects.size(); ++i) {
        area += mNonOverlapRects[i].Area();
    }
    return area;
}

template <class T>
void RectUnionArea<T>::AddRect(const Rect<T>& rect) {
    for (int i = 0; i < mNonOverlapRects.size(); ++i) {
        if (rect.IsOverlap(mNonOverlapRects[i])) {
            CutRect(mNonOverlapRects[i], rect);
            mNonOverlapRects[i] = mNonOverlapRects.back();
            mNonOverlapRects.pop_back();
        }
    }
    mNonOverlapRects.push_back(rect);
}

template <class T>
void RectUnionArea<T>::CutRect(const Rect<T>& rectCut, const Rect<T>& rectRef) {
    T x1 = rectCut.X(), x2 = rectCut.X() + rectCut.Width();
    T y1 = rectCut.Y(), y2 = rectCut.Y() + rectCut.Height();
    T k1 = Math::Max(rectRef.X(), x1);
    T k2 = Math::Min(rectRef.X() + rectRef.Width(), x2);
    if (x1 < k1) {
        mNonOverlapRects.push_back(Rect<T>(x1, y1, k1 - x1, y2 - y1));
    }
    if (x2 > k2) {
        mNonOverlapRects.push_back(Rect<T>(k2, y1, x2 - k2, y2 - y1));
    }
    x1 = k1; x2 = k2;
    k1 = Math::Max(rectRef.Y(), y1);
    k2 = Math::Min(rectRef.Y() + rectRef.Height(), y2);
    if (y1 < k1) {
        mNonOverlapRects.push_back(Rect<T>(x1, y1, x2 - x1, k1 - y1));
    }
    if (y2 > k2) {
        mNonOverlapRects.push_back(Rect<T>(x1, k2, x2 - x1, y2 - k2));
    }
}

} //~ namespace longan
