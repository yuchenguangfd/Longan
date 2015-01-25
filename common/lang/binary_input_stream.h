/*
 * binary_input_stream.h
 * Created on: Nov 15, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_LANG_BINARY_INPUT_STREAM_H
#define COMMON_LANG_BINARY_INPUT_STREAM_H

#include "types.h"
#include "defines.h"
#include "common/error.h"
#include <string>
#include <cstdio>

namespace longan {

class BinaryInputStream {
public:
	BinaryInputStream();
	BinaryInputStream(const std::string& filename);
	virtual ~BinaryInputStream();
	void Close();
	friend BinaryInputStream& operator>> (BinaryInputStream& bis, uint8& i);
	friend BinaryInputStream& operator>> (BinaryInputStream& bis, int32& i);
	friend BinaryInputStream& operator>> (BinaryInputStream& bis, int64& i);
	friend BinaryInputStream& operator>> (BinaryInputStream& bis, float32& f);
	friend BinaryInputStream& operator>> (BinaryInputStream& bis, float64& f);
	void Read(int32 *data, int size);
	void Read(float32 *data, int size);
	void Read(float64 *data, int size);
protected:
	FILE* mStream;
	DISALLOW_COPY_AND_ASSIGN(BinaryInputStream);
};

inline BinaryInputStream& operator>> (BinaryInputStream& bis, uint8& i) {
    if (fread((void*)&i, (uint64)sizeof(uint8), 1, bis.mStream) != 1) {
        throw LonganFileReadError();
    }
    return bis;
}

inline BinaryInputStream& operator>> (BinaryInputStream& bis, int32& i) {
    if (fread((void*)&i, (uint64)sizeof(int32), 1, bis.mStream) != 1) {
        throw LonganFileReadError();
    }
    return bis;
}

inline BinaryInputStream& operator>> (BinaryInputStream& bis, int64& i) {
    if (fread((void*)&i, (uint64)sizeof(int64), 1, bis.mStream) != 1) {
        throw LonganFileReadError();
    }
    return bis;
}

inline BinaryInputStream& operator>> (BinaryInputStream& bis, float32& f) {
    if (fread((void*)&f, (uint64)sizeof(float32), 1, bis.mStream) != 1) {
        throw LonganFileReadError();
    }
    return bis;
}

inline BinaryInputStream& operator>> (BinaryInputStream& bis, float64& f) {
    if (fread((void*)&f, (uint64)sizeof(float64), 1, bis.mStream) != 1) {
        throw LonganFileReadError();
    }
    return bis;
}

} //~ namespace longan

#endif /* COMMON_LANG_BINARY_INPUT_STREAM_H */
