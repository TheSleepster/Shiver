/* date = June 18th 2024 7:15 pm */

#ifndef _SHIVER_ARRAY_H
#define _SHIVER_ARRAY_H

#include "../Intrinsics.h"
#include "MemoryArena.h"

struct ShiverArray
{
    uint32 MaxElements;
    uint32 ElementCount;
    char* Data;
};

internal inline ShiverArray
sh_InitArray(void *Data, size_t SizeInBytes, uint32 ElementCount, MemoryArena *Memory)
{
    ShiverArray ReturnArray = {};
    
    ReturnArray.Data = (char *)ArenaAlloc(Memory, ElementCount * SizeInBytes);
    ReturnArray.MaxElements = ElementCount;
    ReturnArray.ElementCount = 0;
    
    return(ReturnArray);
}

internal inline void
sh_DeleteArray(ShiverArray *Array, MemoryArena *Memory)
{
    ArenaDealloc(Memory, (void *)Array->Data);
}

internal inline void
sh_ArrayAdd(ShiverArray *Array, char *Type)
{
    if(Array->ElementCount < Array->MaxElements)
    {
        Array->Data[Array->ElementCount] = *Type;
        ++Array->ElementCount;
    }
    else
    {
        Assert(false, "Array's Maximum capacity Exceeded... Returning early\n");
    }
}

// TODO(Sleepster): Do we need to decrement the element counter here as well?
internal inline void
sh_ArrayRemoveAtIndex(ShiverArray *Array, uint32 Index)
{
    if(Index <= 0)
    {
        Array->Data[Index] = 0;
    }
    else
    {
        Trace("Invalid Array Index!... Returning early!\n");
        return;
    }
}

internal inline void
sh_ArrayRemoveTopIndex(ShiverArray *Array)
{
    Array->Data[Array->ElementCount] = 0;
    --Array->ElementCount;
}

internal inline bool
sh_IsIndexNull(ShiverArray *Array, uint32 Index)
{
    if(Array->Data[Index] == 0)
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

internal inline int32
sh_GetArrayElementCount(ShiverArray *Array)
{
    return(Array->ElementCount);
}

internal inline void
sh_ClearArrayData(ShiverArray *Array)
{
    for(uint32 Index = 0;
        Index < Array->ElementCount;
        ++Index)
    {
        Array->Data[Index] = 0;
    }
}

#endif //_SHIVER_ARRAY_H