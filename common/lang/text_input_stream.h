/*
 * text_input_stream.h
 * Created on: Jan 24, 2015
 * Author: chenguangyu
 */

#ifndef COMMON_LANG_TEXT_INPUT_STREAM_H
#define COMMON_LANG_TEXT_INPUT_STREAM_H

#include "types.h"
#include "defines.h"
#include "common/error.h"
#include <string>
#include <cstdio>

namespace longan {

class TextInputStream {
public:
    TextInputStream();
    TextInputStream(const std::string& filename);
	virtual ~TextInputStream();
	void Close();
	friend TextInputStream& operator>> (TextInputStream& bis, char& c);
	friend TextInputStream& operator>> (TextInputStream& bis, int32& i);
	friend TextInputStream& operator>> (TextInputStream& bis, int64& i);
	friend TextInputStream& operator>> (TextInputStream& bis, float32& f);
	friend TextInputStream& operator>> (TextInputStream& bis, float64& f);
	void Read(int32 *data, int size, int splitLen = 1);
	void Read(float32 *data, int size, int splitLen = 1);
	void Read(float64 *data, int size, int splitLen = 1);
protected:
	FILE* mStream;
	DISALLOW_COPY_AND_ASSIGN(TextInputStream);
};

inline TextInputStream& operator>> (TextInputStream& tis, char& c) {
    if (fscanf(tis.mStream, "%c", &c) != 1) {
        throw LonganFileReadError();
    }
    return tis;
}

inline TextInputStream& operator>> (TextInputStream& tis, int32& i) {
    if (fscanf(tis.mStream, "%d", &i) != 1) {
        throw LonganFileReadError();
    }
    return tis;
}

inline TextInputStream& operator>> (TextInputStream& tis, int64& i) {
    if (fscanf(tis.mStream, "%lld", &i) != 1) {
        throw LonganFileReadError();
    }
    return tis;
}

inline TextInputStream& operator>> (TextInputStream& tis, float32& f) {
    if (fscanf(tis.mStream, "%f", &f) != 1) {
        throw LonganFileReadError();
    }
    return tis;
}

inline TextInputStream& operator>> (TextInputStream& tis, float64& f) {
    if (fscanf(tis.mStream, "%lf", &f) != 1) {
        throw LonganFileReadError();
    }
    return tis;
}

} //~ namespace longan

#endif /* COMMON_LANG_TEXT_INPUT_STREAM_H */
