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

#define DECLARE_LONGAN_ERROR(ErrorName, DefaultMessage) \
    class ErrorName : public LonganError { \
    public: \
        ErrorName(const char* const message = DefaultMessage) : LonganError(message) { } \
    };

DECLARE_LONGAN_ERROR(LonganRuntimeError, "Runtime Error");
DECLARE_LONGAN_ERROR(LonganConfigError, "Config Error");

DECLARE_LONGAN_ERROR(LonganFileOpenError, "File Open Error");
DECLARE_LONGAN_ERROR(LonganFileReadError, "File Read Error");
DECLARE_LONGAN_ERROR(LonganFileWriteError, "File Write Error");
DECLARE_LONGAN_ERROR(LonganFileDeleteError, "File Delete Error");
DECLARE_LONGAN_ERROR(LonganFileFormatError, "File Format Error");

DECLARE_LONGAN_ERROR(LonganNotSupportError, "Not Support Error");

}  // namespace longan

#endif /* COMMON_ERROR_H */
