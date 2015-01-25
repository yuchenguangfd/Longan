/*
 * text_output_stream.h
 * Created on: Jan 24, 2015
 * Author: chenguangyu
 */

#ifndef COMMON_LANG_TEXT_OUTPUT_STREAM_H
#define COMMON_LANG_TEXT_OUTPUT_STREAM_H

#include "types.h"
#include "defines.h"
#include "common/error.h"
#include <string>
#include <cstdio>

namespace longan {

class TextOutputStream {
public:
    TextOutputStream();
	TextOutputStream(const std::string& filename);
	virtual ~TextOutputStream();
	void Close();
	friend TextOutputStream& operator<< (TextOutputStream& tos, char c);
	friend TextOutputStream& operator<< (TextOutputStream& tos, int32 i);
	friend TextOutputStream& operator<< (TextOutputStream& tos, int64 i);
	friend TextOutputStream& operator<< (TextOutputStream& tos, float32 f);
	friend TextOutputStream& operator<< (TextOutputStream& tos, float64 f);
	void Write(const int32 *data, int size, const char *split = ",");
	void Write(const float32 *data, int size, const char *split = ",");
	void Write(const float64 *data, int size, const char *split = ",");
protected:
	FILE* mStream;
	DISALLOW_COPY_AND_ASSIGN(TextOutputStream);
};

inline TextOutputStream& operator<< (TextOutputStream& tos, char c) {
    if (fprintf(tos.mStream, "%c", c) < 0) {
        throw LonganFileWriteError();
    }
    return tos;
}

inline TextOutputStream& operator<< (TextOutputStream& tos, int32 i) {
    if (fprintf(tos.mStream, "%d", i) < 0) {
        throw LonganFileWriteError();
    }
    return tos;
}

inline TextOutputStream& operator<< (TextOutputStream& tos, int64 i) {
    if (fprintf(tos.mStream, "%lld", i) < 0) {
        throw LonganFileWriteError();
    }
    return tos;
}

inline TextOutputStream& operator << (TextOutputStream& tos, float32 f) {
    if (fprintf(tos.mStream, "%f", f) < 0) {
        throw LonganFileWriteError();
    }
    return tos;
}

inline TextOutputStream& operator << (TextOutputStream& tos, float64 f) {
    if (fprintf(tos.mStream, "%lf", f) < 0) {
        throw LonganFileWriteError();
    }
    return tos;
}

} //~ namespace longan

#endif /* COMMON_LANG_TEXT_OUTPUT_STREAM_H */
