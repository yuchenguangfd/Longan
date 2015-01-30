#include "bp_network.h"
#include "bp_network_computation.h"
#include "ml/base/math.h"
#include "common/lang/binary_input_stream.h"
#include "common/lang/binary_output_stream.h"
#include "common/logging/logging.h"
#include <cassert>

namespace longan {

BpNetwork::BpNetwork(const BpNetworkArchitecture& arch) :
    mArchitecture(arch),
    mNumLayer(arch.NumLayer()) {
    mWeights.resize(mNumLayer-1);
    mBiases.resize(mNumLayer-1);
    for (int i = 0; i < mNumLayer-1; ++i) {
        int rows = mArchitecture.NumLayerUnit(i+1);
        int cols = mArchitecture.NumLayerUnit(i);
        mWeights[i] = std::move(Matrix64F(rows, cols));
        mBiases[i] = std::move(Vector64F(rows));
    }
}

BpNetwork::~BpNetwork() { }

Vector64F BpNetwork::Predict(const Vector64F& input) {
    assert(input.Size() == mArchitecture.NumInputLayerUnit());
    Vector64F output = input;
    for (int i = 1; i < mNumLayer; ++i) {
        output = Sigmoid(mWeights[i-1] * output + mBiases[i-1]);
    }
    return std::move(output);
}

void BpNetwork::Train(const BpNetworkTrainOption *trainOption,
        const SupervisedDatamodel *datamodel) {
    assert(datamodel->FeatureDim() == mArchitecture.NumInputLayerUnit());
    assert(datamodel->TargetDim() == mArchitecture.NumOutputLayerUnit());
    Log::I("ml", "BpNetwork::Train()");
    BpNetworkComputation *computationDelegate;
    if (trainOption->Accelerate()) {
        computationDelegate = new BpNetworkComputationSimple();
    } else {
        computationDelegate = new BpNetworkComputationSimple();
    }
    computationDelegate->Train(this, trainOption, datamodel);
    delete computationDelegate;
}

void BpNetwork::Load(const std::string& filename) {
    BinaryInputStream bis(filename);
    int numLayer;
    bis >> numLayer;
    assert(numLayer == mArchitecture.NumLayer());
    for (int i = 0; i < numLayer; ++i) {
        int numLayerUnit;
        bis >> numLayerUnit;
        assert(numLayerUnit == mArchitecture.NumLayerUnit(i));
    }
    for (int i = 0; i < numLayer - 1; ++i) {
        bis.Read(mWeights[i].Data(), mWeights[i].Size());
        bis.Read(mBiases[i].Data(), mBiases[i].Size());
    }
}

void BpNetwork::Save(const std::string& filename) const {
    BinaryOutputStream bos(filename);
    bos << mArchitecture.NumLayer();
    for (int i = 0; i < mArchitecture.NumLayer(); ++i) {
        bos << mArchitecture.NumLayerUnit(i);
    }
    for (int i = 0; i < mArchitecture.NumLayer() - 1; ++i) {
        bos.Write(mWeights[i].Data(), mWeights[i].Size());
        bos.Write(mBiases[i].Data(), mBiases[i].Size());
   }
}

} //~ namespace longan
