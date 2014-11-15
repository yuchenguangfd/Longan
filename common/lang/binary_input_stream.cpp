/*
 * binary_input_stream.cpp
 * Created on: Nov 15, 2014
 * Author: chenguangyu
 */

#include "binary_input_stream.h"

namespace longan {

BinaryInputStream::BinaryInputStream() :
	mStream(stdin) {
}

BinaryInputStream::BinaryInputStream(const std::string& filename) {
	mStream = fopen(filename.c_str(), "rb");
	if (mStream == nullptr) {
		throw LonganFileOpenError();
	}
}

BinaryInputStream::~BinaryInputStream() {
    if (mStream != nullptr) {
        Close();
    }
}

void BinaryInputStream::Close() {
    if (mStream != stdin) {
        fclose(mStream);
    }
    mStream = nullptr;
}

} //~ namespace longan
