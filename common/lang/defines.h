/*
 * defines.h
 * Created on: Nov 15, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_LANG_DEFINES_H
#define COMMON_LANG_DEFINES_H

#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
    ClassName(const ClassName&) = delete; \
    void operator=(const ClassName&) = delete;

#define forever  for (;;)
#define dprint(expr)  printf(#expr " = %g\n", expr)

#endif // COMMON_LANG_DEFINES_H
