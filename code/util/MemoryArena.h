#pragma once
#include "../Intrinsics.h"

// NOTE(Sleepster): Still have no idea if this freelist Idea actually works
struct FreeList 
{
    char *FreeMemory;
    struct FreeList *NextChunk;
};

struct MemoryArena 
{
    uint64 Capacity;
    uint64 Used;
    char* Memory;
    
    FreeList *FreeList;
};

internal inline MemoryArena 
MakeMemoryArena(uint64 Size) 
{
    MemoryArena MemoryArena = {};
    MemoryArena.Memory = (char *)malloc(Size);
    if(MemoryArena.Memory) 
    {
        MemoryArena.Capacity = Size;
        MemoryArena.FreeList = 0;
        memset(MemoryArena.Memory, 0, Size);
    }
    return(MemoryArena);
}

// TODO(Sleepster): Check that the allignment size is actually correct
internal inline char *
ArenaAlloc(MemoryArena *MemoryArena, uint64 Size) 
{
    char *Result = nullptr;
    uint64 AllignedSize = (Size + 7) & ~ 7;
    if(MemoryArena->FreeList) 
    {
        FreeList *Chunk = MemoryArena->FreeList;
        MemoryArena->FreeList = Chunk->NextChunk;
        
        return((char *)Chunk->FreeMemory);
    }
    
    if(MemoryArena->Used + AllignedSize <= MemoryArena->Capacity) 
    {
        Result = MemoryArena->Memory + MemoryArena->Used;
        MemoryArena->Used += AllignedSize;
    }
    return(Result);
}

internal inline void
ArenaDealloc(MemoryArena *MemoryArena, void *Data) 
{
    FreeList *FreeList = {};
    FreeList->FreeMemory = (char *)Data;
    FreeList->NextChunk = MemoryArena->FreeList;
    MemoryArena->FreeList = FreeList;
}

internal inline void 
ArenaReset(MemoryArena *MemoryArena) 
{
    MemoryArena->Used = 0;
    MemoryArena->FreeList = 0;
}

internal inline void 
ArenaDestroy(MemoryArena *MemoryArena) 
{
    free(MemoryArena);
    MemoryArena = nullptr;
}
