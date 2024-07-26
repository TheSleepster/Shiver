/* date = July 25 2024 01:18 am*/

#ifndef SH_STRINGS_H
#define SH_STRINGS_H

#include "../Intrinsics.h"
#include "MemoryArena.h"

// NOTE(Sleepster): Length Based Strings
struct string
{
    uint64 Length;
    uint8 *Data;
};

#define STR(s) (string{GetStringLength((const char *)s), (uint8 *)s})

const string NULL_STRING = {};

internal inline uint64
GetStringLength(const char *String)
{
    uint64 Length = 0;
    while(*String != 0)
    {
        ++Length;
        ++String;
    }
    return(Length);
}

internal inline bool
StringsMatch(string A, string B)
{
    if(A.Length != B.Length) return(0);
    if(A.Data == B.Data) return(1);

    return(memcmp(A.Data, B.Data, A.Length) == 0);
}

internal inline string
HeapString(MemoryArena *Scratch, uint64 Length)
{
    string S;
    S.Length = Length;
    S.Data = (uint8 *)ArenaAlloc(Scratch, Length);

    return(S);
}

internal inline string
StringCopy(string A, MemoryArena *Scratch)
{
    string C = HeapString(Scratch, A.Length);
    memcpy(C.Data, A.Data, A.Length);

    return(C);
}

internal inline string
ConcatinatePair(const string Left, const string Right, MemoryArena *Scratch)
{
    if(Right.Length + Left.Length == 0) return(NULL_STRING);
    if(Left.Length == 0) return(Right);
    if(Right.Length == 0) return(Left);

    string Result = {};
    Result.Length = Left.Length + Right.Length;
    Result.Data = (uint8 *)ArenaAlloc(Scratch, Result.Length);

    memcpy(Result.Data, Left.Data, Left.Length);
    memcpy(Result.Data + Left.Length, Right.Data, Right.Length);

    return(Result);
}

#endif // _SH_STRINGS_H
