/*
 * cf_auto_encoder_convert.h
 * Created on: Feb 3, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_CONVERT_H
#define RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_CONVERT_H

#include "recsys/base/basic_convert.h"

namespace longan {

class CFAutoEncoderConvert : public BasicConvert {
public:
    CFAutoEncoderConvert(const std::string& ratingTextFilepath, bool needNormalize,
            const std::string& ratingBinaryFilepath);
    virtual void Convert() override;
private:
    bool mNeedNormalize;
};

} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_CONVERT_H */
