/*
 * text_output_stream.cpp
 * Created on: Jan 24, 2015
 * Author: chenguangyu
 */

#include "text_output_stream.h"

namespace longan {

TextOutputStream::TextOutputStream() : mStream(stdout) { }

TextOutputStream::TextOutputStream(const std::string& filename) {
	mStream = fopen(filename.c_str(), "w");
	if (mStream == nullptr) {
		throw LonganFileOpenError();
	}
}

TextOutputStream::~TextOutputStream() {
    if (mStream != nullptr) Close();
}

void TextOutputStream::Close() {
    if (mStream == stdout) {
        fflush(mStream);
        mStream = nullptr;
        return;
    }
    fclose(mStream);
    mStream = nullptr;
}

void TextOutputStream::Write(const int32 *data, int size, const char *split) {
    if (size == 0) return;
    if (fprintf(mStream, "%d", data[0]) < 0) {
        throw LonganFileWriteError();
    }
    for (int i = 1; i < size; ++i) {
        if (fprintf(mStream, "%s%d", split, data[i]) < 0) {
            throw LonganFileWriteError();
        }
    }
}

void TextOutputStream::Write(const float32 *data, int size, const char *split) {
    if (size == 0) return;
    if (fprintf(mStream, "%f", data[0]) < 0) {
        throw LonganFileWriteError();
    }
    for (int i = 1; i < size; ++i) {
        if (fprintf(mStream, "%s%f", split, data[i]) < 0) {
            throw LonganFileWriteError();
        }
    }
}

void TextOutputStream::Write(const float64 *data, int size, const char *split) {
    if (size == 0) return;
    if (fprintf(mStream, "%lf", data[0]) < 0) {
        throw LonganFileWriteError();
    }
    for (int i = 1; i < size; ++i) {
        if (fprintf(mStream, "%s%lf", split, data[i]) < 0) {
            throw LonganFileWriteError();
        }
    }
}

} //~ namespace longan
