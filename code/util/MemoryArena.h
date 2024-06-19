#pragma once
#include "../Intrinsics.h"

// NOTE(Sleepster): Still have no idea if this freelist Idea actually works
struct FreeList 
{
    struct FreeList *NextChunk;
};

struct MemoryArena 
{
    size_t Capacity;
    size_t Used;
    char* Memory;
    
    FreeList *FreeList;
};

internal inline MemoryArena 
MakeMemoryArena(size_t Size) 
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

internal inline char *
ArenaAlloc(MemoryArena *MemoryArena, size_t Size) 
{
    char *Result = nullptr;
    size_t AllignedSize = (Size + 7) & ~ 7;
    if(MemoryArena->FreeList) 
    {
        FreeList *Chunk = MemoryArena->FreeList;
        MemoryArena->FreeList = Chunk->NextChunk;
        
        return((char *)Chunk);
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
    FreeList *FreeList = (struct FreeList *)Data;
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