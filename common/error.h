/*
 * error.h
 * Created on: Jul 25, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_ERROR_H
#define COMMON_ERROR_H

namespace longan {

const int LONGAN_SUCC = 0;
const int LONGAN_FAIL = -1;

const int LONGAN_OBJECT_BUFFER_FAILED = -300;
const int LONGAN_OBJECT_BUFFER_NO_ALIVE_CONSUMER = -301;
const int LONGAN_OBJECT_BUFFER_TRY_PUSH_FAILED = -302;
const int LONGAN_OBJECT_BUFFER_UNEXCEPTED_PRODUCER_QUIT = -303;
const int LONGAN_OBJECT_BUFFER_NO_MORE_OBJECT = -304;
const int LONGAN_OBJECT_BUFFER_TRY_POP_FAILED = -305;
const int LONGAN_OBJECT_BUFFER_UNEXCEPTED_CONSUMER_QUIT = -306;


class LonganError {
public:
    LonganError(const char* const message = nullptr) : mMessage(message) {}
    virtual ~LonganError() {}
    virtual const char* Message() const { return mMessage; }
protected:
    const char* const mMessage;
};

class LonganRuntimeError : public LonganError {
public:
	LonganRuntimeError(const char* const message = "Runtime Error") :
		LonganError(message) { }
};

class LonganFileOpenError : public LonganError {
public:
	LonganFileOpenError(const char* const message = "File Open Error") :
		LonganError(message) { }
};

class LonganFileReadError : public LonganError {
public:
	LonganFileReadError(const char* const message = "File Read Error") :
		LonganError(message) { }
};

class LonganFileWriteError : public LonganError {
public:
	LonganFileWriteError(const char* const message = "File Write Error") :
		LonganError(message) { }
};

class LonganFileDeleteError : public LonganError {
public:
	LonganFileDeleteError(const char* const message = "File Delete Error") :
		LonganError(message) { }
};

class LonganFileFormatError : public LonganError {
public:
    LonganFileFormatError(const char* const message = "File Format Error") :
        LonganError(message) { }
};

class LonganConfigError : public LonganError {
public:
    LonganConfigError(const char* const message = "Config Error") :
        LonganError(message) { }
};

class LonganNotSupportError : public LonganError {
public: LonganNotSupportError(const char* const message = "Not Support Error") :
    LonganError(message) {}
};

}  // namespace longan

#endif /* COMMON_ERROR_H */
