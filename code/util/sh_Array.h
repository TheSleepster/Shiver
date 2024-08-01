/* date = July 31st 2024 6:22 pm */

#ifndef _SH_ARRAY_H
#define _SH_ARRAY_H

#include "../Intrinsics.h"
#include "MemoryArena.h"

template <typename Type, int32 Capacity>
struct array
{
    int32 Count = 0;
    Type Data[Capacity];

    inline Type& 
    operator[](int32 Index)
    {
        Check(Index >= 0, "Invalid Index\n");
        Check(Index < Capacity, "Invalid Index\n");

        return(Data[Index]);
    }

    int32 
    add(Type Element)
    {
        Check(Count >= 0, "Invalid Count\n");
        Check(Count < Capacity, "Array Full\n");

        Data[Count] = Element;
        return(++Count);
    }

    inline void
    remove(int32 Index)
    {
        Check(Index >= 0, "Invalid Index\n");
        Check(Index < Count, "Index Is Invalid\n");

        Data[Index] = Data[--Count];
    }

    inline void
    clear()
    {
        Count = 0;
    }

    inline bool
    full()
    {
        return(Count >= Capacity);
    }
};

// TODO(Sleepster): make a 2d array
#if 0
template <typename Type, int32 CapacityX, int32 CapacityY>
struct array2d
{
    int32 Count = 0;
    Type Data[Capacity];

    inline Type& 
    operator[](int32 Index)
    {
        Check(Index >= 0, "Invalid Index\n");
        Check(Index < Capacity, "Invalid Index\n");

        return(Data[Index]);
    }

    int32 
    add(Type Element)
    {
        Check(Count >= 0, "Invalid Count\n");
        Check(Count < CapacityY, "Array Full\n");

        Data[Count] = Element;
        return(++Count);
    }

    inline void
    remove(int32 Index)
    {
        Check(Index >= 0, "Invalid Index\n");
        Check(Index < Count, "Index Is Invalid\n");

        Data[Index] = Data[--Count];
    }

    inline void
    clear()
    {
        Count = 0;
    }

    inline bool
    full()
    {
        return(Count >= Capacity);
    }
};
#endif

#endif //_SH_ARRAY_H
