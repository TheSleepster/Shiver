/* date = June 15th 2024 7:04 am */

#ifndef _INTRINSICS_H
#define _INTRINSICS_H

#if SHIVER_SLOW

#define Assert(Expression, Message) if(!(Expression)) {OutputDebugStringA(Message); DebugBreak();}
#define Check(Expression, Message) if(!(Expression)) {OutputDebugStringA(Message);}
#define dAssert(Expression) if(!(Expression)) {DebugBreak();}
#define Trace(Message) {OutputDebugStringA(Message);}
#define print_m(Message, ...) {char BUFFER[256] = {}; sprintf(BUFFER, Message, __VA_ARGS__); OutputDebugStringA(BUFFER);}

#else

#define Assert(Expression, Message)
#define Check(Expression, Message)
#define dAssert(Expression)
#define print_m(Message, Printing)
#define Trace(Message)

#endif

#define Kilobytes(Value) ((uint64)(Value) * 1024)
#define Megabytes(Value) ((uint64)Kilobytes(Value) * 1024)
#define Gigabytes(Value) ((uint64)Megabytes(Value) * 1024)

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

#define Min(A, B) ((A < B) ? (A) : (B))
#define Max(A, B) ((A > B)) ? (A) : (B))

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define global_variable static
#define local_persist static
#define internal static

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int8 bool8;
typedef int32 bool32;

typedef float real32;
typedef double real64;

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define ENGINE 1
#define SHIVER_SLOW 1
#define FMOD 0

#endif //_INTRINSICS_H
