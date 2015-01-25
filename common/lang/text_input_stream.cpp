/*
 * text_input_stream.cpp
 * Created on: Jan 25, 2014
 * Author: chenguangyu
 */

#include "text_input_stream.h"

namespace longan {

TextInputStream::TextInputStream() :
	mStream(stdin) {
}

TextInputStream::TextInputStream(const std::string& filename) {
	mStream = fopen(filename.c_str(), "r");
	if (mStream == nullptr) {
		throw LonganFileOpenError();
	}
}

TextInputStream::~TextInputStream() {
    if (mStream != nullptr) {
        Close();
    }
}

void TextInputStream::Close() {
    if (mStream != stdin) {
        fclose(mStream);
    }
    mStream = nullptr;
}

void TextInputStream::Read(int32 *data, int size, int splitLen) {
    if (size == 0) return;
    if (fscanf(mStream, "%d", &data[0]) != 1) {
        throw LonganFileReadError();
    }
    for (int i = 1; i < size; ++i) {
        for (int j = 0; j < splitLen; ++j) fgetc(mStream);
        if (fscanf(mStream, "%d", &data[i]) != 1) {
            throw LonganFileReadError();
        }
    }
}

void TextInputStream::Read(float32 *data, int size, int splitLen) {
    if (size == 0) return;
    if (fscanf(mStream, "%f", &data[0]) != 1) {
        throw LonganFileReadError();
    }
    for (int i = 1; i < size; ++i) {
        for (int j = 0; j < splitLen; ++j) fgetc(mStream);
        if (fscanf(mStream, "%f", &data[i]) != 1) {
            throw LonganFileReadError();
        }
    }
}

void TextInputStream::Read(float64 *data, int size, int splitLen) {
    if (size == 0) return;
    if (fscanf(mStream, "%lf", &data[0]) != 1) {
        throw LonganFileReadError();
    }
    for (int i = 1; i < size; ++i) {
        for (int j = 0; j < splitLen; ++j) fgetc(mStream);
        if (fscanf(mStream, "%lf", &data[i]) != 1) {
            throw LonganFileReadError();
        }
    }
}

} //~ namespace longan
