/*
 * datamodel.cpp
 * Created on: Jan 21, 2015
 * Author: chenguangyu
 */

#include "datamodel.h"
#include "common/lang/text_input_stream.h"
#include "common/lang/binary_input_stream.h"
#include "common/lang/text_output_stream.h"
#include "common/lang/binary_output_stream.h"

namespace longan {

Datamodel::Datamodel(int featureDim) :
    mFeatureDim(featureDim) { }

Datamodel::Datamodel(int featureDim, int reservedCapacity) :
    mFeatureDim(featureDim) {
    mFeatures.reserve(reservedCapacity);
}

Datamodel::Datamodel(Datamodel&& orig) noexcept :
    mFeatureDim(orig.mFeatureDim),
    mFeatures(std::move(orig.mFeatures)){
}

Datamodel::~Datamodel() { }

Datamodel& Datamodel::operator= (Datamodel&& rhs) noexcept {
    if (this == &rhs) return *this;
    mFeatureDim = rhs.mFeatureDim;
    mFeatures = std::move(rhs.mFeatures);
    return *this;
}

Matrix64F Datamodel::GetDesignMatrix() const {
    Matrix64F DM(mFeatures.size(), mFeatureDim);
    for (int i = 0; i < mFeatures.size(); ++i) {
        for (int j = 0; j < mFeatureDim; ++j) {
            DM[i][j] = mFeatures[i][j];
        }
    }
    return std::move(DM);
}

Datamodel Datamodel::LoadFromTextFile(const std::string& filename) {
    TextInputStream tis(filename);
    int numSample, featureDim;
    char split;
    tis >> numSample >> split >> featureDim >> split;
    Datamodel model(featureDim, numSample);
    Vector64F feature(featureDim);
    for (int i = 0; i < numSample; ++i) {
        tis.Read(feature.Data(), feature.Size(), 1);
        tis >> split;
        model.AddSample(feature);
    }
    return std::move(model);
}

Datamodel Datamodel::LoadFromBinaryFile(const std::string& filename) {
    BinaryInputStream bis(filename);
    int numSample, featureDim;
    bis >> numSample >> featureDim;
    Datamodel model(featureDim, numSample);
    Vector64F feature(featureDim);
    for (int i = 0; i < numSample; ++i) {
        bis.Read(feature.Data(), feature.Size());
        model.AddSample(feature);
    }
    return std::move(model);
}

void Datamodel::WriteToTextFile(const Datamodel& model, const std::string& filename) {
    TextOutputStream tos(filename);
    tos << model.NumSample() << ',' << model.FeatureDim() << '\n';
    for (int i = 0; i < model.NumSample(); ++i) {
        const Vector64F& feature = model.Feature(i);
        tos.Write(feature.Data(), feature.Size(), ",");
        tos << '\n';
    }
}

void Datamodel::WriteToBinaryFile(const Datamodel& model, const std::string& filename) {
    BinaryOutputStream bos(filename);
    bos << model.NumSample() << model.FeatureDim();
    for (int i = 0; i < model.NumSample(); ++i) {
        const Vector64F& feature = model.Feature(i);
        bos.Write(feature.Data(), feature.Size());
    }
}

ClassificationDatamodel::ClassificationDatamodel(int featureDim, int numClass) :
    Datamodel(featureDim),
    mNumClass(numClass) { }

ClassificationDatamodel::ClassificationDatamodel(int featureDim, int numClass, int reservedCapacity) :
    Datamodel(featureDim),
    mNumClass(numClass) {
    mLabels.reserve(reservedCapacity);
}

ClassificationDatamodel::ClassificationDatamodel(ClassificationDatamodel&& orig) noexcept :
    Datamodel(std::move(orig)),
    mNumClass(orig.mNumClass),
    mLabels(std::move(orig.mLabels)) { }

ClassificationDatamodel::~ClassificationDatamodel() { }

ClassificationDatamodel& ClassificationDatamodel::operator= (ClassificationDatamodel&& rhs) noexcept {
    if (this == &rhs) return *this;
    mFeatureDim = rhs.mFeatureDim;
    mFeatures = std::move(rhs.mFeatures);
    mNumClass = rhs.mNumClass;
    mLabels = std::move(rhs.mLabels);
    return *this;
}

ClassificationDatamodel ClassificationDatamodel::LoadFromTextFile(const std::string& filename) {
    TextInputStream tis(filename);
    int numSample, featureDim, numClass;
    char split;
    tis >> numSample >> split
        >> featureDim >> split
        >> numClass >> split;
    ClassificationDatamodel model(featureDim, numClass, numSample);
    Vector64F feature(featureDim);
    int label;
    for (int i = 0; i < numSample; ++i) {
        tis.Read(feature.Data(), feature.Size(), 1);
        tis >> split >> label >> split;
        model.AddSample(feature, label);
    }
    return std::move(model);
}

ClassificationDatamodel ClassificationDatamodel::LoadFromBinaryFile(const std::string& filename) {
    BinaryInputStream bis(filename);
    int numSample, featureDim, numClass;
    bis >> numSample >> featureDim >> numClass;
    ClassificationDatamodel model(featureDim, numClass, numSample);
    Vector64F feature(featureDim);
    for (int i = 0; i < numSample; ++i) {
        bis.Read(feature.Data(), feature.Size());
        int label;
        bis >> label;
        model.AddSample(feature, label);
    }
    return std::move(model);
}

void ClassificationDatamodel::WriteToTextFile(const ClassificationDatamodel& model, const std::string& filename) {
    TextOutputStream tos(filename);
    tos << model.NumSample() << ',' << model.FeatureDim() << ',' << model.NumClass() << '\n';
    for (int i = 0; i < model.NumSample(); ++i) {
        const Vector64F& feature = model.Feature(i);
        tos.Write(feature.Data(), feature.Size(), ",");
        tos << '|' << model.Label(i) << '\n';
    }
}

void ClassificationDatamodel::WriteToBinaryFile(const ClassificationDatamodel& model, const std::string& filename) {
    BinaryOutputStream bos(filename);
    bos << model.NumSample() << model.FeatureDim() << model.NumClass();
    for (int i = 0; i < model.NumSample(); ++i) {
        const Vector64F& feature = model.Feature(i);
        bos.Write(feature.Data(), feature.Size());
        bos << model.Label(i);
    }
}

RegressionDatamodel::RegressionDatamodel(int featureDim) :
    Datamodel(featureDim) { }

RegressionDatamodel::RegressionDatamodel(int featureDim, int reservedCapacity) :
    Datamodel(featureDim) {
    mValues.reserve(reservedCapacity);
}

RegressionDatamodel::RegressionDatamodel(RegressionDatamodel&& orig) noexcept :
    Datamodel(std::move(orig)),
    mValues(std::move(orig.mValues)) { }

RegressionDatamodel::~RegressionDatamodel() { }

RegressionDatamodel& RegressionDatamodel::operator= (RegressionDatamodel&& rhs) noexcept {
    if (this == &rhs) return *this;
    mFeatureDim = rhs.mFeatureDim;
    mFeatures = std::move(rhs.mFeatures);
    mValues = std::move(rhs.mValues);
    return *this;
}

RegressionDatamodel RegressionDatamodel::LoadFromTextFile(const std::string& filename) {
    TextInputStream tis(filename);
    int numSample, featureDim;
    char split;
    tis >> numSample >> split
        >> featureDim >> split;
    RegressionDatamodel model(featureDim, numSample);
    Vector64F feature(featureDim);
    double value;
    for (int i = 0; i < numSample; ++i) {
        tis.Read(feature.Data(), feature.Size(), 1);
        tis >> split >> value >> split;
        model.AddSample(feature, value);
    }
    return std::move(model);
}

RegressionDatamodel RegressionDatamodel::LoadFromBinaryFile(const std::string& filename) {
    BinaryInputStream bis(filename);
    int numSample, featureDim;
    bis >> numSample >> featureDim;
    RegressionDatamodel model(featureDim, numSample);
    Vector64F feature(featureDim);
    for (int i = 0; i < numSample; ++i) {
        bis.Read(feature.Data(), feature.Size());
        double value;
        bis >> value;
        model.AddSample(feature, value);
    }
    return std::move(model);
}

void RegressionDatamodel::WriteToTextFile(const RegressionDatamodel& model, const std::string& filename) {
    TextOutputStream tos(filename);
    tos << model.NumSample() << ',' << model.FeatureDim() << '\n';
    for (int i = 0; i < model.NumSample(); ++i) {
        const Vector64F& feature = model.Feature(i);
        tos.Write(feature.Data(), feature.Size(), ",");
        tos << '|' << model.Value(i) << '\n';
    }
}

void RegressionDatamodel::WriteToBinaryFile(const RegressionDatamodel& model, const std::string& filename) {
    BinaryOutputStream bos(filename);
    bos << model.NumSample() << model.FeatureDim();
    for (int i = 0; i < model.NumSample(); ++i) {
        const Vector64F& feature = model.Feature(i);
        bos.Write(feature.Data(), feature.Size());
        bos << model.Value(i);
    }
}

Vector64F RegressionDatamodel::GetValueVector() const {
    Vector64F vec(mValues.begin(), mValues.end());
    return std::move(vec);
}

SupervisedDatamodel::SupervisedDatamodel(int featureDim, int targetDim) :
    Datamodel(featureDim),
    mTargetDim(targetDim) { }

SupervisedDatamodel::SupervisedDatamodel(int featureDim, int targetDim, int reservedCapacity) :
    Datamodel(featureDim, reservedCapacity),
    mTargetDim(targetDim) {
    mTargets.reserve(reservedCapacity);
}

SupervisedDatamodel::SupervisedDatamodel(SupervisedDatamodel&& orig) noexcept :
    Datamodel(std::move(orig)),
    mTargetDim(orig.mTargetDim),
    mTargets(std::move(orig.mTargets)) { }

SupervisedDatamodel::SupervisedDatamodel(ClassificationDatamodel&& orig) noexcept :
    Datamodel(std::move(orig)),
    mTargetDim(orig.NumClass()) {
    mTargets.resize(mFeatures.size());
    for (int i = 0; i < mFeatures.size(); ++i) {
        int label = orig.Label(i);
        Vector64F target(mTargetDim, true, 0.0);
        target[label] = 1.0;
        mTargets[i] = std::move(target);
    }
}

SupervisedDatamodel::~SupervisedDatamodel() { }

SupervisedDatamodel& SupervisedDatamodel::operator= (SupervisedDatamodel&& rhs) noexcept {
    if (this == &rhs) return *this;
    mFeatureDim = rhs.mFeatureDim;
    mFeatures = std::move(rhs.mFeatures);
    mTargetDim = rhs.mTargetDim;
    mTargets = std::move(rhs.mTargets);
    return *this;
}

SupervisedDatamodel SupervisedDatamodel::LoadFromTextFile(const std::string& filename) {
    TextInputStream tis(filename);
    int numSample, featureDim, targetDim;
    char split;
    tis >> numSample >> split
        >> featureDim >> split
        >> targetDim >> split;
    SupervisedDatamodel model(featureDim, targetDim, numSample);
    Vector64F feature(featureDim);
    Vector64F target(targetDim);
    for (int i = 0; i < numSample; ++i) {
        tis.Read(feature.Data(), feature.Size(), 1);
        tis >> split;
        tis.Read(target.Data(), target.Size(), 1);
        tis >> split;
        model.AddSample(feature, target);
    }
    return std::move(model);
}

SupervisedDatamodel SupervisedDatamodel::LoadFromBinaryFile(const std::string& filename) {
    BinaryInputStream bis(filename);
    int numSample, featureDim, targetDim;
    bis >> numSample >> featureDim >> targetDim;
    SupervisedDatamodel model(featureDim, targetDim, numSample);
    Vector64F feature(featureDim);
    Vector64F target(targetDim);
    for (int i = 0; i < numSample; ++i) {
        bis.Read(feature.Data(), feature.Size());
        bis.Read(target.Data(), target.Size());
        model.AddSample(feature, target);
    }
    return std::move(model);
}

void SupervisedDatamodel::WriteToTextFile(const SupervisedDatamodel& model, const std::string& filename) {
    TextOutputStream tos(filename);
    tos << model.NumSample() << ',' << model.FeatureDim() << ',' << model.TargetDim() << '\n';
    for (int i = 0; i < model.NumSample(); ++i) {
        const Vector64F& feature = model.Feature(i);
        tos.Write(feature.Data(), feature.Size(), ",");
        tos << '|';
        const Vector64F& target = model.Target(i);
        tos.Write(target.Data(), target.Size());
        tos << '\n';
    }
}

void SupervisedDatamodel::WriteToBinaryFile(const SupervisedDatamodel& model, const std::string& filename) {
    BinaryOutputStream bos(filename);
    bos << model.NumSample() << model.FeatureDim() << model.TargetDim();
    for (int i = 0; i < model.NumSample(); ++i) {
        const Vector64F& feature = model.Feature(i);
        bos.Write(feature.Data(), feature.Size());
        const Vector64F& target = model.Target(i);
        bos.Write(target.Data(), target.Size());
    }
}

} //~ namespace longan
