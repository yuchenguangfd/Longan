/*
 * image.h
 * Created on: Jan 25, 2015
 * Author: chenguangyu
 */

#ifndef CV_BASE_IMAGE_H
#define CV_BASE_IMAGE_H

#include "common/lang/types.h"

namespace longan {

/* image base interface */
template <class PixelValueType>
class Image {
public:
    virtual ~Image() { }
    virtual int Width() const = 0;
    virtual int Height() const = 0;
    virtual int NumChannel() const = 0;
    virtual PixelValueType GetPixel(int x, int y, int channel = 0) const = 0;
    virtual void SetPixel(int x, int y, PixelValueType value, int channel = 0) = 0;
    virtual void Release() = 0;
    virtual void Load(const std::string& filename) = 0;
    virtual void Save(const std::string& filename) const = 0;
};

typedef Image<uint8> Image8U;
typedef Image<float32> Image32F;
typedef Image<float64> Image64F;

} //~ namespace longan

#endif /* CV_BASE_IMAGE_H */
