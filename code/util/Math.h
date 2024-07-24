#pragma once

#include "../Intrinsics.h"
#include <stdint.h>
#include <math.h>

typedef float real32;
typedef double real64;
typedef int32_t int32;
typedef int64_t int64;

#define PI 3.14159265358979323846264338327950288

#define DegToRad(Degrees) ((Degrees) * (PI / 180.0))
#define RadToDeg(Radians) ((Radians) * (180.0 / PI))

#define Min(A, B) ((A < B) ? (A) : (B))
#define Max(A, B) ((A > B)) ? (A) : (B))

// FLOAT VECTOR 2
struct vec2
{
    real32 x;
    real32 y;
};

// OPERATOR OVERLOADING

internal inline vec2
operator-(vec2 A, vec2 B)
{
    vec2 Result = {};
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    
    return(Result);
}

internal inline vec2
operator-(vec2 A)
{
    vec2 Result = {};
    Result.x = -A.x;
    Result.y = -A.y;
    
    return(Result);
}

internal inline vec2
operator+(vec2 A, vec2 B)
{
    vec2 Result = {};
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    
    return(Result);
}

internal inline vec2
operator-=(vec2 &A, real32 B)
{
    vec2 Result = {};
    A.x = A.x - B;
    A.y = A.y - B;
    
    return(Result);
}

internal inline vec2
operator*(vec2 A, vec2 B)
{
    vec2 Result = {};
    Result.x = A.x * B.x;
    Result.y = A.y * B.y;
    
    return(Result);
}

internal inline vec2
operator*(real32 A, vec2 B)
{
    vec2 Result = {};
    Result.x = A * B.x;
    Result.y = A * B.y;
    
    return(Result);
}

internal inline vec2
operator*(vec2 A, real32 B)
{
    vec2 Result = {};
    Result.x = A.x * B;
    Result.y = A.y * B;
    return(Result);
}

internal inline vec2
operator*=(vec2 &B, real32 A)
{
    B = A * B;
    return(B);
}

internal inline vec2
operator/(vec2 A, vec2 B)
{
    vec2 Result = {};
    
    Result.x = A.x / B.x;
    Result.y = A.y / B.y;
    
    return(Result);
}

internal inline vec2
operator/(vec2 A, real32 B)
{
    vec2 Result = {};
    
    Result.x = A.x / B;
    Result.y = A.y / B;
    
    return(Result);
}

internal inline vec2
operator+=(vec2 &A, vec2 B)
{
    A.x = A.x + B.x;
    A.y = A.y + B.y;
    
    return(A);
}

// FLOAT OPERATIONS

internal inline real32
v2Square(real32 A)
{
    real32 Result = A * A;
    return(Result);
}

internal inline real32
v2Dot(vec2 A, vec2 B)
{
    real32 Result = A.x*B.x + A.y*B.y;
    return(Result);
}

internal inline real32
v2Cross(vec2 A, vec2 B)
{
    real32 Result = A.x*B.y - A.y*B.x;
    return(Result);
}
// NOTE(Sleepster): It's magnitude but I can't be bothered to change it now
internal inline real32
v2LengthSq(vec2 A)
{
    real32 Result = v2Dot(A, A);
    return(Result);
}

internal inline real32
v2Length(vec2 A)
{
    real32 Result = sqrtf(v2LengthSq(A));
    return(Result);
}

internal inline real32
v2Distance(vec2 A, vec2 B)
{
    return(v2Length(A - B));
}

internal inline vec2
v2Normalize(vec2 A)
{
    vec2 Result = {};
    real32 Length = v2Length(A);
    if(Length == 0)
    {
        Length = 1;
    }
    
    Result.x = A.x / Length;
    Result.y = A.y / Length;
    return(Result);
}

internal vec2
v2Reflect(vec2 A, vec2 B)
{
    vec2 Result = {};
    real32 Dot = v2Dot(A, B);
    
    Result.x = A.x - (2 * Dot * B.x);
    Result.y = A.y - (2 * Dot * B.y);
    return(Result);
}

internal inline real32
LerpR32(real32 A, real32 B, real32 T)
{
    return(A + (B - A) * T);
}

internal inline vec2
v2Lerp(vec2 A, vec2 B, real32 T)
{
    vec2 Result = {};
    Result.x = LerpR32(A.x, B.x, T);
    Result.y = LerpR32(A.y, B.y, T);
    
    return(Result);
}

internal inline real32
MaxR32(real32 a, real32 b)
{
    if(a > b)
    {
        return(a);
    }
    return(b);
}

internal inline real32
MinR32(real32 a, real32 b)
{
    if(a < b)
    {
        return(a);
    }
    return(b);
}

internal inline real32
Square(real32 A)
{
    real32 Result = A * A;
    return(Result);
}

internal inline vec2
v2Perp(vec2 A)
{
    vec2 Result = {-A.y, A.x};
    return(Result);
}

internal inline vec2
v2Inverse(vec2 A)
{
    vec2 Result = {-A.x, -A.y};
    return(Result);
}

internal inline vec2
v2Invert(vec2 A)
{
    vec2 Result = {A.x * -1, A.y * -1};
    return(Result);
}

internal inline vec2
v2TripleProduct(vec2 A, vec2 B, vec2 C)
{
    vec2 Result;
    
    real32 AC = (A.x*C.x) + (A.y*C.y);
    real32 BC = (B.x*C.x) + (B.y*C.y);
    Result.x = (B.x*AC) - (A.x*BC);
    Result.y = (B.y*AC) - (A.y*BC);
    
    return(Result);
}

internal inline bool
operator<(vec2 A, vec2 B)
{
    real32 Ax = v2Length(A);
    real32 Bx = v2Length(B);
    if(Ax < Bx)
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

internal inline bool
operator>(vec2 A, vec2 B)
{
    real32 Ax = v2Length(A);
    real32 Bx = v2Length(B);
    if(Ax > Bx)
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

internal inline vec2
v2Min(vec2 A, vec2 B)
{
    if(A < B)
    {
        return(A);
    }
    return(B);
}


internal inline vec2
v2Max(vec2 A, vec2 B)
{
    if(A < B)
    {
        return(A);
    }
    return(B);
}

internal inline real32
ClampR32(real32 Value, real32 Min, real32 Max)
{
    const real32 Val = Value < Min ? Min : Value;
    return(Val > Max ? Max : Val);
}

// FLOAT 2x2 MATRIX
union mat2
{
    real32 Elements[2][2];
    vec2 Columns[2];
};

// FLOAT 2x2 MATRIX OVERLOADING

internal inline mat2
operator+(mat2 A, mat2 B)
{
    mat2 Result = {};
    
    Result.Elements[0][0] = A.Elements[0][0] + B.Elements[0][0];
    Result.Elements[0][1] = A.Elements[0][1] + B.Elements[0][1];
    Result.Elements[1][0] = A.Elements[1][0] + B.Elements[1][0];
    Result.Elements[1][1] = A.Elements[1][1] + B.Elements[1][1];
    
    return(Result);
}

internal inline mat2
operator-(mat2 A, mat2 B)
{
    mat2 Result = {};
    
    Result.Elements[0][0] = A.Elements[0][0] - B.Elements[0][0];
    Result.Elements[0][1] = A.Elements[0][1] - B.Elements[0][1];
    Result.Elements[1][0] = A.Elements[1][0] - B.Elements[1][0];
    Result.Elements[1][1] = A.Elements[1][1] - B.Elements[1][1];
    
    return(Result);
}

// FLOAT 2x2 MATRIX FUNCTIONS
internal inline mat2
m2Transpose(mat2 A)
{
    mat2 Result = {};
    
    Result.Elements[0][1] = A.Elements[1][0];
    Result.Elements[1][0] = A.Elements[0][1];
    
    return(Result);
}

internal inline vec2
Multiplym2v2(mat2 A, vec2 B)
{
    vec2 Result = {};
    
    Result.x = B.x * A.Columns[0].x;
    Result.y = B.x * A.Columns[1].x;
    Result.y = B.y * A.Columns[0].y;
    Result.y = B.y * A.Columns[1].y;
    
    return(Result);
}

internal inline mat2
Multiplym2m2(mat2 A, mat2 B)
{
    mat2 Result = {};
    
    Result.Columns[0] = Multiplym2v2(A, B.Columns[0]);
    Result.Columns[1] = Multiplym2v2(A, B.Columns[1]);
    
    return(Result);
}

internal inline mat2
Multiplym2real32(mat2 A, real32 B)
{
    mat2 Result = {};
    
    Result.Elements[0][0] = A.Elements[0][0] * B;
    Result.Elements[0][1] = A.Elements[0][1] * B;
    Result.Elements[1][0] = A.Elements[1][0] * B;
    Result.Elements[1][1] = A.Elements[1][1] * B;
    
    return(Result);
}

internal inline mat2
Dividem2real32(mat2 A, real32 B)
{
    mat2 Result = {};
    
    Result.Elements[0][0] = A.Elements[0][0] / B;
    Result.Elements[0][1] = A.Elements[0][1] / B;
    Result.Elements[1][0] = A.Elements[1][0] / B;
    Result.Elements[1][1] = A.Elements[1][1] / B;
    
    return(Result);
}

internal inline real32
m2Determinant(mat2 A)
{
    return(A.Elements[0][0] * A.Elements[1][1] - A.Elements[0][1] * A.Elements[1][0]);
}

internal inline mat2
v2InvDeterminate(mat2 A)
{
    mat2 Result = {};
    
    real32 InverseDeterminate = 1.0f / m2Determinant(A);
    
    Result.Elements[0][0] = InverseDeterminate * +A.Elements[1][1];
    Result.Elements[1][1] = InverseDeterminate * +A.Elements[0][0];
    Result.Elements[0][1] = InverseDeterminate * -A.Elements[0][1];
    Result.Elements[1][0] = InverseDeterminate * -A.Elements[1][0];
    
    return(Result);
}

// INTEGER VECTOR 2

struct ivec2
{
    int x;
    int y;
};

// OPERATOR OVERLOADING

internal inline ivec2
operator-(ivec2 A, ivec2 B)
{
    ivec2 Result = {};
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    
    return(Result);
}

internal inline ivec2
operator-(ivec2 A, int32 B)
{
    ivec2 Result = {0};
    
    Result.x = A.x - B;
    Result.y = A.y - B;
    
    return(Result);
}

internal inline ivec2
operator+(ivec2 A, ivec2 B)
{
    ivec2 Result = {};
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    
    return(Result);
}

internal inline ivec2
operator*(ivec2 A, ivec2 B)
{
    ivec2 Result = {};
    Result.x = A.x * B.x;
    Result.y = A.y * B.y;
    
    return(Result);
}

internal inline ivec2
operator*(ivec2 A, int B)
{
    ivec2 Result = {0};
    
    Result.x = int32(A.x * B);
    Result.y = int32(A.y * B);
    
    return(Result);
}

internal inline ivec2
operator*(ivec2 A, real32 B)
{
    ivec2 Result = {};
    Result.x = int32(A.x * B);
    Result.y = int32(A.y * B);
    
    return(Result);
}

internal inline ivec2
operator/(ivec2 A, ivec2 B)
{
    ivec2 Result = {};
    Result.x = A.x / B.x;
    Result.y = A.y / B.y;
    
    return(Result);
}

internal inline ivec2
operator/(ivec2 A, real32 B)
{
    ivec2 Result = {};
    
    Result.x = int32(A.x * B);
    Result.y = int32(A.y * B);
    
    return(Result);
}

internal inline ivec2
operator*=(ivec2 &A, int32 B)
{
    A.x = A.x * B;
    A.y = A.y * B;
    return(A);
}

// INTEGER OPERATIONS

internal inline int64
maxi64(int64 a, int64 b)
{
    if(a > b)
    {
        return(a);
    }
    return(b);
}

internal inline int32
iSquare (int32 A)
{
    int32 Result = A * A;
    return(Result);
}

internal inline ivec2
iVLerp(ivec2 A, ivec2 B, real32 T)
{
    ivec2 Result = {};
    Result.x = int32(LerpR32((real32)A.x, (real32)B.x, T));
    Result.y = int32(LerpR32((real32)A.y, (real32)B.y, T));
    
    return(Result);
}

internal inline vec2
v2Cast(ivec2 A)
{
    vec2 Result = {};
    
    Result.x = real32(A.x);
    Result.y = real32(A.y);
    
    return(Result);
}

internal inline vec2
MultiplyIV2Real(ivec2 A, vec2 B)
{
    vec2 Result = {};
    vec2 Cast = v2Cast(A);
    
    Result.x = Cast.x * B.x;
    Result.y = Cast.y * B.y;
    
    return(Result);
}

internal inline ivec2
iv2Cast(vec2 A)
{
    ivec2 Result = {};
    
    Result.x = int32(A.x);
    Result.y = int32(A.y);
    
    return(Result);
}

internal inline vec2
v2Convert(ivec2 A)
{
    return(vec2{real32(A.x), real32(A.y)});
}

internal inline bool
Equals(real32 A, real32 B, real32 Tolerance)
{
    return(fabs(A - B) <= Tolerance);
}

internal inline void
Approach(real32 *Value, real32 Target, real32 Rate, real32 Delta_t)
{
    *Value += real32((Target - *Value) * (1.0 - pow(2.0f, -Rate * Delta_t)));
    if(Equals(*Value, Target, 0.001f))
    {
        *Value = Target;
    }
}

internal inline void
v2Approach(vec2 *Value, vec2 Target, real32 Rate, real32 Delta_t)
{
    Approach(&(Value->x), Target.x, Rate, Delta_t);
    Approach(&(Value->y), Target.y, Rate, Delta_t);
}

// FLOAT VECTOR 3

struct vec3
{
    real32 x;
    real32 y;
    real32 z;
};

struct ivec3
{
    union
    {
        int32 Elements[3];
        
        struct
        {
            int32 x;
            int32 y;
            int32 z;
        };
        
        struct
        {
            int32 Width;
            int32 Height;
            int32 Channels;
        };
    };
};

// VECTOR3 OVERLOADING

internal inline vec3
operator-(vec3 A, vec3 B)
{
    vec3 Result = {};
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    
    return(Result);
}

internal inline vec3
operator-(vec3 A)
{
    vec3 Result = {};
    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;
    
    return(Result);
}

internal inline vec3
operator+(vec3 A, vec3 B)
{
    vec3 Result = {};
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    
    return(Result);
}

internal inline vec3
operator*(vec3 A, vec3 B)
{
    vec3 Result = {};
    Result.x = A.x * B.x;
    Result.y = A.y * B.y;
    Result.z = A.z * B.z;
    
    return(Result);
}

internal inline vec3
operator*(vec3 A, real32 B)
{
    vec3 Result = {};
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    
    return(Result);
}

internal inline vec3
operator*=(vec3 A, real32 B)
{
    vec3 Result = {};

    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.x * B;

    return(Result);
}

internal inline vec3
operator/(vec3 A, vec3 B)
{
    vec3 Result = {};
    
    Result.x = A.x / B.x;
    Result.y = A.y / B.y;
    Result.z = A.z / B.z;
    
    return(Result);
}

internal inline vec3
operator/(vec3 A, real32 B)
{
    vec3 Result = {};
    
    Result.x = A.x / B;
    Result.y = A.y / B;
    Result.z = A.z / B;
    
    return(Result);
}

// FLOAT VECTOR3 FUNCTIONS

internal inline vec3
MultiplyV3Real(vec3 A, real32 B)
{
    vec3 Result = {};
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    
    return(Result);
}

internal inline vec3
v3Cross(vec3 A, vec3 B)
{
    vec3 Result = {};
    
    Result.x = (A.y * B.z) - (A.z * B.y);
    Result.y = (A.z * B.x) - (A.x * B.z);
    Result.z = (A.x * B.y) - (A.y * B.x);
    
    return(Result);
}

internal inline real32
v3Dot(vec3 A, vec3 B)
{
    return((A.x * B.x) + (A.y * B.y) + (A.z * B.z));
}

union mat3
{
    real32 Elements[3][3];
    vec3 Columns[3];
};

// FLOAT 3x3 MATRIX OVERLOADING

internal inline mat3
operator+(mat3 A, mat3 B)
{
    mat3 Result = {};
    
    Result.Elements[0][0] = A.Elements[0][0] + B.Elements[0][0];
    Result.Elements[0][1] = A.Elements[0][1] + B.Elements[0][1];
    Result.Elements[0][2] = A.Elements[0][2] + B.Elements[0][2];
    Result.Elements[1][0] = A.Elements[1][0] + B.Elements[1][0];
    Result.Elements[1][1] = A.Elements[1][1] + B.Elements[1][1];
    Result.Elements[1][2] = A.Elements[1][2] + B.Elements[1][2];
    Result.Elements[2][0] = A.Elements[2][0] + B.Elements[2][0];
    Result.Elements[2][1] = A.Elements[2][1] + B.Elements[2][1];
    Result.Elements[2][2] = A.Elements[2][2] + B.Elements[2][2];
    
    return(Result);
}

internal inline mat3
operator-(mat3 A, mat3 B)
{
    mat3 Result = {};
    
    Result.Elements[0][0] = A.Elements[0][0] - B.Elements[0][0];
    Result.Elements[0][1] = A.Elements[0][1] - B.Elements[0][1];
    Result.Elements[0][2] = A.Elements[0][2] - B.Elements[0][2];
    Result.Elements[1][0] = A.Elements[1][0] - B.Elements[1][0];
    Result.Elements[1][1] = A.Elements[1][1] - B.Elements[1][1];
    Result.Elements[1][2] = A.Elements[1][2] - B.Elements[1][2];
    Result.Elements[2][0] = A.Elements[2][0] - B.Elements[2][0];
    Result.Elements[2][1] = A.Elements[2][1] - B.Elements[2][1];
    Result.Elements[2][2] = A.Elements[2][2] - B.Elements[2][2];
    
    return(Result);
}

// FLOAT 3x3 MATRIX FUNCTIONS

internal inline mat3
m3Transpose(mat3 A)
{
    mat3 Result = A;
    
    Result.Elements[0][1] = A.Elements[1][0];
    Result.Elements[0][2] = A.Elements[2][0];
    Result.Elements[1][0] = A.Elements[0][1];
    Result.Elements[1][2] = A.Elements[2][1];
    Result.Elements[2][1] = A.Elements[1][2];
    Result.Elements[2][0] = A.Elements[0][2];
    
    return(Result);
}

internal inline vec3
MultiplyM3V3(mat3 A, vec3 B)
{
    vec3 Result = {};
    
    Result.x = B.x * A.Columns[0].x;
    Result.y = B.x * A.Columns[0].y;
    Result.z = B.x * A.Columns[0].z;
    
    Result.x += B.y * A.Columns[1].x;
    Result.y += B.y * A.Columns[1].y;
    Result.z += B.y * A.Columns[1].z;
    
    Result.x += B.z * A.Columns[2].x;
    Result.y += B.z * A.Columns[2].y;
    Result.z += B.z * A.Columns[2].z;
    
    return(Result);
}

internal inline mat3
MultiplyM3M3(mat3 A, mat3 B)
{
    mat3 Result;
    Result.Columns[0] = MultiplyM3V3(A, B.Columns[0]);
    Result.Columns[1] = MultiplyM3V3(A, B.Columns[1]);
    Result.Columns[2] = MultiplyM3V3(A, B.Columns[2]);
    
    return(Result);
}

internal inline mat3
MultiplyM3Real(mat3 A, real32 B)
{
    mat3 Result = {};
    
    Result.Elements[0][0] = A.Elements[0][0] * B;
    Result.Elements[0][1] = A.Elements[0][1] * B;
    Result.Elements[0][2] = A.Elements[0][2] * B;
    Result.Elements[1][0] = A.Elements[1][0] * B;
    Result.Elements[1][1] = A.Elements[1][1] * B;
    Result.Elements[1][2] = A.Elements[1][2] * B;
    Result.Elements[2][0] = A.Elements[2][0] * B;
    Result.Elements[2][1] = A.Elements[2][1] * B;
    Result.Elements[2][2] = A.Elements[2][2] * B;
    
    return(Result);
}

internal inline mat3
DivideM3Real(mat3 A, real32 B)
{
    mat3 Result = {};
    
    Result.Elements[0][0] = A.Elements[0][0] / B;
    Result.Elements[0][1] = A.Elements[0][1] / B;
    Result.Elements[0][2] = A.Elements[0][2] / B;
    Result.Elements[1][0] = A.Elements[1][0] / B;
    Result.Elements[1][1] = A.Elements[1][1] / B;
    Result.Elements[1][2] = A.Elements[1][2] / B;
    Result.Elements[2][0] = A.Elements[2][0] / B;
    Result.Elements[2][1] = A.Elements[2][1] / B;
    Result.Elements[2][2] = A.Elements[2][2] / B;
    
    return(Result);
}

internal inline real32
m3Determinate(mat3 A)
{
    mat3 Result = {};
    
    Result.Columns[0] = v3Cross(A.Columns[1], A.Columns[2]);
    Result.Columns[1] = v3Cross(A.Columns[2], A.Columns[0]);
    Result.Columns[2] = v3Cross(A.Columns[0], A.Columns[1]);
    
    return(v3Dot(Result.Columns[2], A.Columns[2]));
}

internal inline mat3
m3InvDeterminate(mat3 A)
{
    mat3 Cross = {};
    Cross.Columns[0] = v3Cross(A.Columns[1], A.Columns[2]);
    Cross.Columns[1] = v3Cross(A.Columns[2], A.Columns[0]);
    Cross.Columns[2] = v3Cross(A.Columns[0], A.Columns[1]);
    
    real32 InverseDeterminate = 1.0f / v3Dot(Cross.Columns[2], A.Columns[2]);
    
    mat3 Result = {};
    Result.Columns[0] = MultiplyV3Real(Cross.Columns[0], InverseDeterminate);
    Result.Columns[1] = MultiplyV3Real(Cross.Columns[1], InverseDeterminate);
    Result.Columns[2] = MultiplyV3Real(Cross.Columns[2], InverseDeterminate);
    
    return(m3Transpose(Result));
}

// FLOAT VECTOR 4

union vec4
{
    real32 Elements[4];
    
    union 
    {
        vec3 xyz;
        struct 
        {
            real32 x, y, z;      
        };
        real32 w;
    };
    
    struct
    {
        real32 r;
        real32 g;
        real32 b;
        real32 a;
    };
    
    struct
    {
        real32 Left;
        real32 Right;
        real32 Top;
        real32 Bottom;
    };
};

// INTEGER VECTOR 4
union ivec4
{
    real32 Elements[4];
    
    struct
    {
        int32 x;
        int32 y;
        int32 Width;
        int32 Height;
    };
};

// VECTOR4 OVERLOADING

// TODO : These are large and expensive calls, check SIMD or ASM functions
internal inline vec4
operator+(vec4 A, vec4 B)
{
    vec4 Result = {};
    
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    Result.w = A.w + B.w;
    
    return(Result);
}

internal inline vec4
operator-(vec4 A, vec4 B)
{
    vec4 Result = {};
    
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    Result.w = A.w - B.w;
    
    return(Result);
}

internal inline vec4
operator*(vec4 a, vec4 b)
{
    vec4 result = {};
    
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    result.w = a.w * b.w;
    
    return(result);
}

internal inline vec4
operator*(vec4 A, real32 B)
{
    vec4 Result = {};
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    Result.w = A.w * B;
    
    return(Result);
}

internal inline vec4
operator/(vec4 A, vec4 B)
{
    vec4 Result = {};
    
    Result.x = A.x / B.x;
    Result.y = A.y / B.y;
    Result.z = A.z / B.z;
    Result.w = A.w / B.w;
    
    return(Result);
}

internal inline vec4
operator/(vec4 A, real32 B)
{
    vec4 Result = {};
    
    Result.x = A.x / B;
    Result.y = A.y / B;
    Result.z = A.z / B;
    Result.w = A.w / B;
    
    return(Result);
}

// VECTOR4 FUNCTIONS
internal inline vec4
v4Create(vec3 A, real32 W)
{
    vec4 Result = {};

    Result.xyz = A;
    Result.w = W;

    return(Result);
}

// 4x4 FLOAT MATRIX

union mat4
{
    real32 Elements[4][4];
    vec4 Columns[4];
};

// 4x4 FLOAT MATRIX OPERATORS

internal inline mat4
operator+(mat4 A, mat4 B)
{
    mat4 Result = {};
    
    Result.Columns[0] = A.Columns[0] + B.Columns[0];
    Result.Columns[1] = A.Columns[1] + B.Columns[1];
    Result.Columns[2] = A.Columns[2] + B.Columns[2];
    Result.Columns[3] = A.Columns[3] + B.Columns[3];
}

internal inline mat4
operator-(mat4 A, mat4 B)
{
    mat4 Result = {};
    
    Result.Columns[0] = A.Columns[0] - B.Columns[0];
    Result.Columns[1] = A.Columns[1] - B.Columns[1];
    Result.Columns[2] = A.Columns[2] - B.Columns[2];
    Result.Columns[3] = A.Columns[3] - B.Columns[3];
}

// 4x4 FLOAT MATRIX FUNCTIONS

internal inline real32
mat4Determinant(mat4 A)
{
    mat4 Result = {};
    
    vec3 Temp1 = v3Cross(A.Columns[0].xyz, A.Columns[1].xyz);
    vec3 Temp2 = v3Cross(A.Columns[2].xyz, A.Columns[3].xyz);
    vec3 Temp3 =  (A.Columns[0].xyz * A.Columns[1].w) - (A.Columns[1].xyz * A.Columns[0].w);
    vec3 Temp4 = (A.Columns[2].xyz * A.Columns[3].w) - (A.Columns[3].xyz * A.Columns[2].w);

    return(v3Dot(Temp1, Temp4) + v3Dot(Temp2, Temp3));
}

internal inline vec4
mat4Transform(vec4 A, mat4 B)
{
    vec4 Result = {};
    
    Result.x = A.Elements[0] * B.Columns[0].x;
    Result.y = A.Elements[0] * B.Columns[0].y;
    Result.z = A.Elements[0] * B.Columns[0].z;
    Result.w = A.Elements[0] * B.Columns[0].w;
    
    Result.x += A.Elements[1] * B.Columns[1].x;
    Result.y += A.Elements[1] * B.Columns[1].y;
    Result.z += A.Elements[1] * B.Columns[1].z;
    Result.w += A.Elements[1] * B.Columns[1].w;
    
    Result.x += A.Elements[2] * B.Columns[2].x;
    Result.y += A.Elements[2] * B.Columns[2].y;
    Result.z += A.Elements[2] * B.Columns[2].z;
    Result.w += A.Elements[2] * B.Columns[2].w;
    
    Result.x += A.Elements[3] * B.Columns[3].x;
    Result.y += A.Elements[3] * B.Columns[3].y;
    Result.z += A.Elements[3] * B.Columns[3].z;
    Result.w += A.Elements[3] * B.Columns[3].w;
    
    return(Result);
}

// TODO : SIMD for this please omfg this is expensive
internal inline mat4
mat4Transpose(mat4 A)
{
    mat4 Result = {};
    
    Result.Elements[0][0] = A.Elements[0][0];
    Result.Elements[0][1] = A.Elements[1][0];
    Result.Elements[0][2] = A.Elements[2][0];
    Result.Elements[0][3] = A.Elements[3][0];
    Result.Elements[1][0] = A.Elements[0][1];
    Result.Elements[1][1] = A.Elements[1][1];
    Result.Elements[1][2] = A.Elements[2][1];
    Result.Elements[1][3] = A.Elements[3][1];
    Result.Elements[2][0] = A.Elements[0][2];
    Result.Elements[2][1] = A.Elements[1][2];
    Result.Elements[2][2] = A.Elements[2][2];
    Result.Elements[2][3] = A.Elements[3][2];
    Result.Elements[3][0] = A.Elements[0][3];
    Result.Elements[3][1] = A.Elements[1][3];
    Result.Elements[3][2] = A.Elements[2][3];
    Result.Elements[3][3] = A.Elements[3][3];
    
    return(Result);
}

// TODO : Running out of "Check SIMD expensive"'s

internal inline mat4
mat4ScalerDiv(mat4 A, real32 B)
{
    mat4 Result = {};
    
    Result.Elements[0][0] = A.Elements[0][0] / B;
    Result.Elements[0][1] = A.Elements[0][1] / B;
    Result.Elements[0][2] = A.Elements[0][2] / B;
    Result.Elements[0][3] = A.Elements[0][3] / B;
    Result.Elements[1][0] = A.Elements[1][0] / B;
    Result.Elements[1][1] = A.Elements[1][1] / B;
    Result.Elements[1][2] = A.Elements[1][2] / B;
    Result.Elements[1][3] = A.Elements[1][3] / B;
    Result.Elements[2][0] = A.Elements[2][0] / B;
    Result.Elements[2][1] = A.Elements[2][1] / B;
    Result.Elements[2][2] = A.Elements[2][2] / B;
    Result.Elements[2][3] = A.Elements[2][3] / B;
    Result.Elements[3][0] = A.Elements[3][0] / B;
    Result.Elements[3][1] = A.Elements[3][1] / B;
    Result.Elements[3][2] = A.Elements[3][2] / B;
    Result.Elements[3][3] = A.Elements[3][3] / B;
    
    return(Result);
}

internal inline mat4
mat4ScalerMult(mat4 A, real32 B)
{
    mat4 Result = {};
    
    Result.Elements[0][0] = A.Elements[0][0] * B;
    Result.Elements[0][1] = A.Elements[0][1] * B;
    Result.Elements[0][2] = A.Elements[0][2] * B;
    Result.Elements[0][3] = A.Elements[0][3] * B;
    Result.Elements[1][0] = A.Elements[1][0] * B;
    Result.Elements[1][1] = A.Elements[1][1] * B;
    Result.Elements[1][2] = A.Elements[1][2] * B;
    Result.Elements[1][3] = A.Elements[1][3] * B;
    Result.Elements[2][0] = A.Elements[2][0] * B;
    Result.Elements[2][1] = A.Elements[2][1] * B;
    Result.Elements[2][2] = A.Elements[2][2] * B;
    Result.Elements[2][3] = A.Elements[2][3] * B;
    Result.Elements[3][0] = A.Elements[3][0] * B;
    Result.Elements[3][1] = A.Elements[3][1] * B;
    Result.Elements[3][2] = A.Elements[3][2] * B;
    Result.Elements[3][3] = A.Elements[3][3] * B;
    
    return(Result);
}

internal inline mat4
mat4Multiply(mat4 A, mat4 B)
{
    mat4 Result = {};
    
    Result.Columns[0] = mat4Transform(B.Columns[0], A);
    Result.Columns[1] = mat4Transform(B.Columns[1], A);
    Result.Columns[2] = mat4Transform(B.Columns[2], A);
    Result.Columns[3] = mat4Transform(B.Columns[3], A);
    
    return(Result);
}

internal mat4
mat4Inverse(mat4 A)
{
    mat4 Result = {};

    vec3 C01 = v3Cross(A.Columns[0].xyz, A.Columns[1].xyz);
    vec3 C23 = v3Cross(A.Columns[2].xyz, A.Columns[3].xyz);
    vec3 B10 =  (A.Columns[0].xyz * A.Columns[1].w) - (A.Columns[1].xyz * A.Columns[0].w);
    vec3 B32 = (A.Columns[2].xyz * A.Columns[3].w) - (A.Columns[3].xyz * A.Columns[2].w);

    real32 InvDeterminant = 1.0f / (v3Dot(C01, B32) + v3Dot(C23, B10));

    C01 *= InvDeterminant;
    C23 *= InvDeterminant;
    B10 *= InvDeterminant;
    B32 *= InvDeterminant;

    Result.Columns[0] = v4Create((v3Cross(A.Columns[1].xyz, B32) + (C23 * A.Columns[1].w)), -(v3Dot(A.Columns[1].xyz, C23)));
    Result.Columns[1] = v4Create((v3Cross(B32, A.Columns[0].xyz) - (C23 * A.Columns[0].w)), +(v3Dot(A.Columns[0].xyz, C23)));
    Result.Columns[2] = v4Create((v3Cross(A.Columns[3].xyz, B10) + (C01 * A.Columns[3].w)), -(v3Dot(A.Columns[3].xyz, C01)));
    Result.Columns[3] = v4Create((v3Cross(B10, A.Columns[2].xyz) - (C01 * A.Columns[2].w)), +(v3Dot(A.Columns[2].xyz, C01)));

    return(Result);
}
// TODO : Finish all of the 4x4 matrix calcs
