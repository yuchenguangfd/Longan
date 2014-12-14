/*
 * svd_model.h
 * Created on: Dec 13, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_SVD_SVD_MODEL_H
#define RECSYS_SVD_SVD_MODEL_H

#include <string>
#include <vector>

namespace longan {

struct SVDParameter {
    int dim;
    float lp, lq, lub, lib, gamma;
    SVDParameter() : dim(40), lp(1), lq(1), lub(-1), lib(-1), gamma(0.001) {}
};

class SVDModelPredict {
public:
    SVDModelPredict();
    ~SVDModelPredict();
    void Load(const std::string& filename);
public:
    SVDParameter mParam;
    int mNumUser;
    int mNumItem;
    float mAvg;
    float *P;
    float *Q;
    std::vector<float> UB;
    std::vector<float> IB;
};

} //~ namespace longan

#endif /* RECSYS_SVD_SVD_MODEL_H */
