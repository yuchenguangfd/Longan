/*
 * image_opencv.h
 * Created on: Jan 25, 2015
 * Author: chenguangyu
 */

#ifndef CV_BASE_IMAGE_OPENCV_H
#define CV_BASE_IMAGE_OPENCV_H

#include "image.h"
#include "common/error.h"
#include <opencv2/highgui/highgui.hpp>

namespace longan {

/* implementation of Image by wrap OpenCV cv::Mat */
template <class PixelValueType>
class ImageOpenCV: public Image<PixelValueType> {
public:
    ImageOpenCV();
    ImageOpenCV(int width, int height, int numChannel = 1);
    virtual ~ImageOpenCV();
    virtual int Width() const override { return mImageMat.cols; }
    virtual int Height() const override { return mImageMat.rows; }
    virtual int NumChannel() const override { return mImageMat.channels(); }
    virtual PixelValueType GetPixel(int x, int y, int channel = 0) const override {
        return mImageMat.at<PixelValueType>(y, x, channel);
    }
    virtual void SetPixel(int x, int y, PixelValueType value, int channel = 0) override {
        mImageMat.at<PixelValueType>(y, x, channel) = value;
    }
    virtual void Release() override {
        mImageMat.release();
    }
    virtual void Load(const std::string& filename) override;
    virtual void Save(const std::string& filename) const override;
protected:
    cv::Mat mImageMat;
};

template <class PixelValueType>
inline ImageOpenCV<PixelValueType>::ImageOpenCV() :
    Image<PixelValueType>() { }

template <>
inline ImageOpenCV<uint8>::ImageOpenCV(int width, int height, int numChannel) {
    switch (numChannel) {
        case 1: mImageMat.create(height, width, CV_8UC1); break;
        case 3: mImageMat.create(height, width, CV_8UC3); break;
        default:
            throw LonganRuntimeError();
    }
}

template <class PixelValueType>
inline ImageOpenCV<PixelValueType>::~ImageOpenCV() { }

template <>
inline void ImageOpenCV<uint8>::Load(const std::string& filename) {
    mImageMat = cv::imread(filename);
    if (!mImageMat.data) {
        throw LonganFileOpenError("can not load image file");
    }
}

template <>
inline void ImageOpenCV<uint8>::Save(const std::string& filename) const {
    cv::imwrite(filename, mImageMat);
}

typedef ImageOpenCV<uint8> ImageOpenCV8U;
typedef ImageOpenCV<float32> ImageOpenCV32F;
typedef ImageOpenCV<float64> ImageOpenCV64F;

} //~namespace longan

#endif /* CV_BASE_IMAGE_OPENCV_H*/
