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

#define DEFINE_BINARY_INPUT_STREAM_ONE_READ(TypeName) \
    inline BinaryInputStream& operator>> (BinaryInputStream& bis, TypeName& arg) { \
        if (fread((void*)&arg, (uint64)sizeof(TypeName), 1, bis.mStream) != 1) { \
            throw LonganFileReadError(); \
        } \
        return bis; \
    }

#define DEFINE_BINARY_INPUT_STREAM_MULTIPLE_READ(TypeName) \
    inline void BinaryInputStream::Read(TypeName* data, int size) { \
        if (fread((void*)data, (uint64)sizeof(TypeName), size, mStream) != size) { \
            throw LonganFileReadError(); \
        } \
    }

DEFINE_BINARY_INPUT_STREAM_ONE_READ(uint8)
DEFINE_BINARY_INPUT_STREAM_ONE_READ(int32)
DEFINE_BINARY_INPUT_STREAM_ONE_READ(int64)
DEFINE_BINARY_INPUT_STREAM_ONE_READ(float32)
DEFINE_BINARY_INPUT_STREAM_ONE_READ(float64)

DEFINE_BINARY_INPUT_STREAM_MULTIPLE_READ(int32)
DEFINE_BINARY_INPUT_STREAM_MULTIPLE_READ(float32)
DEFINE_BINARY_INPUT_STREAM_MULTIPLE_READ(float64)

} //~ namespace longan

#endif /* COMMON_LANG_BINARY_INPUT_STREAM_H */
