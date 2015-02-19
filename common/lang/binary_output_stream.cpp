/*
 * binary_output_stream.cpp
 * Created on: Nov 15, 2014
 * Author: chenguangyu
 */

#include "binary_output_stream.h"

namespace longan {

BinaryOutputStream::BinaryOutputStream() : mStream(stdout) { }

BinaryOutputStream::BinaryOutputStream(const std::string& filename) {
	mStream = fopen(filename.c_str(), "wb");
	if (mStream == nullptr) {
		throw LonganFileOpenError();
	}
}

BinaryOutputStream::~BinaryOutputStream() {
    if (mStream != nullptr) Close();
}

void BinaryOutputStream::Close() {
    if (mStream == stdout) {
        fflush(mStream);
        mStream = nullptr;
        return;
    }
    fclose(mStream);
    mStream = nullptr;
}

} //~ namespace longan
