/*
 * svd_convert.h
 * Created on: Dec 11, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_SVD_SVD_CONVERT_H
#define RECSYS_SVD_SVD_CONVERT_H

#include "recsys/base/basic_convert.h"

namespace longan {

class SVDConvert : public BasicConvert {
public:
    using BasicConvert::BasicConvert;
    virtual void Convert() override;
};

} //~ namespace longan

#endif /* RECSYS_SVD_SVD_CONVERT_H */
