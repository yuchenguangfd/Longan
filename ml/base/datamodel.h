/*
 * datamodel.h
 * Created on: Jan 20, 2015
 * Author: chenguangyu
 */

#ifndef ML_BASE_DATAMODEL_H
#define ML_BASE_DATAMODEL_H

#include "common/math/vector.h"
#include "common/math/matrix.h"
#include "common/lang/defines.h"
#include <string>
#include <vector>
#include <cassert>

namespace longan {

class Datamodel {
public:
    Datamodel(int featureDim);
    Datamodel(int featureDim, int reservedCapacity);
    Datamodel(Datamodel&& orig) noexcept;
    virtual ~Datamodel();
    Datamodel& operator= (Datamodel&& rhs) noexcept;
    int FeatureDim() const { return mFeatureDim; }
    int NumSample() const { return mFeatures.size(); }
    const Vector64F& Feature(int i) const { return mFeatures[i]; }
    Vector64F& Feature(int i) { return mFeatures[i]; }
    void AddSample(const Vector64F& feature) {
        assert(feature.Size() == mFeatureDim);
        mFeatures.push_back(feature);
    }
    void Clear() {
        mFeatures.clear();
    }
    Matrix64F GetDesignMatrix() const;
public:
    static Datamodel LoadFromTextFile(const std::string& filename);
    static Datamodel LoadFromBinaryFile(const std::string& filename);
    static void WriteToTextFile(const Datamodel& model, const std::string& filename);
    static void WriteToBinaryFile(const Datamodel& model, const std::string& filename);
protected:
    int mFeatureDim;
    std::vector<Vector64F> mFeatures;
    DISALLOW_COPY_AND_ASSIGN(Datamodel);
};

typedef Datamodel UnsupervisedDatamodel;

class ClassificationDatamodel : public Datamodel {
public:
    ClassificationDatamodel(int featureDim, int numClass);
    ClassificationDatamodel(int featureDim, int numClass, int reservedCapacity);
    ClassificationDatamodel(ClassificationDatamodel&& orig) noexcept;
    virtual ~ClassificationDatamodel();
    ClassificationDatamodel& operator= (ClassificationDatamodel&& rhs) noexcept;
    int NumClass() const { return mNumClass; }
    int Label(int i) const { return mLabels[i]; }
    void AddSample(const Vector64F& feature, int label) {
        assert(feature.Size() == mFeatureDim);
        assert(0 <= label && label < mNumClass);
        mFeatures.push_back(feature);
        mLabels.push_back(label);
    }
    void Clear() {
        mFeatures.clear();
        mLabels.clear();
    }
public:
    static ClassificationDatamodel LoadFromTextFile(const std::string& filename);
    static ClassificationDatamodel LoadFromBinaryFile(const std::string& filename);
    static void WriteToTextFile(const ClassificationDatamodel& model, const std::string& filename);
    static void WriteToBinaryFile(const ClassificationDatamodel& model, const std::string& filename);
protected:
    int mNumClass;
    std::vector<int> mLabels;
};

class RegressionDatamodel : public Datamodel {
public:
    RegressionDatamodel(int featureDim);
    RegressionDatamodel(int featureDim, int reservedCapacity);
    RegressionDatamodel(RegressionDatamodel&& orig) noexcept;
    virtual ~RegressionDatamodel();
    RegressionDatamodel& operator= (RegressionDatamodel&& rhs) noexcept;
    double Value(int i) const { return mValues[i]; }
    void AddSample(const Vector64F& feature, double value) {
        assert(feature.Size() == mFeatureDim);
        mFeatures.push_back(feature);
        mValues.push_back(value);
    }
    void Clear() {
        mFeatures.clear();
        mValues.clear();
    }
    Vector64F GetValueVector() const;
public:
    static RegressionDatamodel LoadFromTextFile(const std::string& filename);
    static RegressionDatamodel LoadFromBinaryFile(const std::string& filename);
    static void WriteToTextFile(const RegressionDatamodel& model, const std::string& filename);
    static void WriteToBinaryFile(const RegressionDatamodel& model, const std::string& filename);
protected:
    std::vector<double> mValues;
};

class SupervisedDatamodel : public Datamodel {
public:
    SupervisedDatamodel(int featureDim, int targetDim);
    SupervisedDatamodel(int featureDim, int targetDim, int reservedCapacity);
    SupervisedDatamodel(SupervisedDatamodel&& orig) noexcept;
    SupervisedDatamodel(ClassificationDatamodel&& orig) noexcept;
    virtual ~SupervisedDatamodel();
    SupervisedDatamodel& operator= (SupervisedDatamodel&& rhs) noexcept;
    int TargetDim() const { return mTargetDim; }
    const Vector64F& Target(int i) const { return mTargets[i]; }
    Vector64F& Target(int i) { return mTargets[i]; }
    void AddSample(const Vector64F& feature, const Vector64F& target) {
        assert(feature.Size() == mFeatureDim);
        assert(target.Size() == mTargetDim);
        mFeatures.push_back(feature);
        mTargets.push_back(target);
    }
    void Clear() {
        mFeatures.clear();
        mTargets.clear();
    }
public:
    static SupervisedDatamodel LoadFromTextFile(const std::string& filename);
    static SupervisedDatamodel LoadFromBinaryFile(const std::string& filename);
    static void WriteToTextFile(const SupervisedDatamodel& model, const std::string& filename);
    static void WriteToBinaryFile(const SupervisedDatamodel& model, const std::string& filename);
protected:
    int mTargetDim;
    std::vector<Vector64F> mTargets;
};

} //~ namespace longan

#endif /* ML_BASE_DATAMODEL_H */
