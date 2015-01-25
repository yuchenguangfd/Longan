/*
 * binary_output_stream.h
 * Created on: Nov 15, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_LANG_BINARY_OUTPUT_STREAM_H
#define COMMON_LANG_BINARY_OUTPUT_STREAM_H

#include "types.h"
#include "defines.h"
#include "common/error.h"
#include <string>
#include <cstdio>

namespace longan {

class BinaryOutputStream {
public:
	BinaryOutputStream();
	BinaryOutputStream(const std::string& filename);
	virtual ~BinaryOutputStream();
	void Close();
	friend BinaryOutputStream& operator<< (BinaryOutputStream& bos, int32 i);
	friend BinaryOutputStream& operator<< (BinaryOutputStream& bos, int64 i);
	friend BinaryOutputStream& operator<< (BinaryOutputStream& bos, float32 f);
	friend BinaryOutputStream& operator<< (BinaryOutputStream& bos, float64 f);
	void Write(const int32 *data, int size);
	void Write(const float32 *data, int size);
	void Write(const float64 *data, int size);
protected:
	FILE* mStream;
	DISALLOW_COPY_AND_ASSIGN(BinaryOutputStream);
};

inline BinaryOutputStream& operator<< (BinaryOutputStream& bos, int32 i) {
    if (fwrite((void*)&i, (uint64)sizeof(int32), 1, bos.mStream) != 1) {
        throw LonganFileWriteError();
    }
    return bos;
}

inline BinaryOutputStream& operator<< (BinaryOutputStream& bos, int64 i) {
    if (fwrite((void*)&i, (uint64)sizeof(int64), 1, bos.mStream) != 1) {
        throw LonganFileWriteError();
    }
    return bos;
}

inline BinaryOutputStream& operator << (BinaryOutputStream& bos, float32 f) {
    if (fwrite((void*)&f, (uint64)sizeof(float32), 1, bos.mStream) != 1) {
        throw LonganFileWriteError();
    }
    return bos;
}

inline BinaryOutputStream& operator << (BinaryOutputStream& bos, float64 f) {
    if (fwrite((void*)&f, (uint64)sizeof(float64), 1, bos.mStream) != 1) {
        throw LonganFileWriteError();
    }
    return bos;
}

} //~ namespace longan

#endif /* COMMON_LANG_BINARY_OUTPUT_STREAM_H */
