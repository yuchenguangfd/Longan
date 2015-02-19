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

#define DEFINE_BINARY_OUTPUT_STREAM_ONE_WRITE(TypeName) \
    inline BinaryOutputStream& operator<< (BinaryOutputStream& bos, TypeName arg) { \
        if (fwrite((void*)&arg, (uint64)sizeof(TypeName), 1, bos.mStream) != 1) { \
            throw LonganFileWriteError(); \
        } \
        return bos; \
    }

#define DEFINE_BINARY_OUTPUT_STREAM_MULTIPLE_WRITE(TypeName) \
    inline void BinaryOutputStream::Write(const TypeName* data, int size) { \
        if (fwrite((void*)data, (uint64)sizeof(TypeName), size, mStream) != size) { \
            throw LonganFileWriteError(); \
        } \
    }

DEFINE_BINARY_OUTPUT_STREAM_ONE_WRITE(int32)
DEFINE_BINARY_OUTPUT_STREAM_ONE_WRITE(int64)
DEFINE_BINARY_OUTPUT_STREAM_ONE_WRITE(float32)
DEFINE_BINARY_OUTPUT_STREAM_ONE_WRITE(float64)

DEFINE_BINARY_OUTPUT_STREAM_MULTIPLE_WRITE(int32)
DEFINE_BINARY_OUTPUT_STREAM_MULTIPLE_WRITE(float32)
DEFINE_BINARY_OUTPUT_STREAM_MULTIPLE_WRITE(float64)

} //~ namespace longan

#endif /* COMMON_LANG_BINARY_OUTPUT_STREAM_H */
