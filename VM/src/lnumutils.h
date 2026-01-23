// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
// This code is based on Lua 5.x implementation licensed under MIT License; see lua_LICENSE.txt for details
#pragma once

#include <math.h>

#define luai_numadd(a, b) ((a) + (b))
#define luai_numsub(a, b) ((a) - (b))
#define luai_nummul(a, b) ((a) * (b))
#define luai_numdiv(a, b) ((a) / (b))
#define luai_numpow(a, b) (pow(a, b))
#define luai_numunm(a) (-(a))
#define luai_numisnan(a) ((a) != (a))
#define luai_numeq(a, b) ((a) == (b))
#define luai_numlt(a, b) ((a) < (b))
#define luai_numle(a, b) ((a) <= (b))
//GIDEROS
#define BINOP(v) ((uint32_t)(((int64_t)(v))&0xFFFFFFFF))
#define luai_nummax(a, b) (fmax(a,b))
#define luai_nummin(a, b) (fmin(a,b))
#define luai_numband(a, b) (BINOP(a)&BINOP(b))
#define luai_numbor(a, b) (BINOP(a)|BINOP(b))
#define luai_numbxor(a, b) (BINOP(a)^BINOP(b))
#define luai_numbsr(a, b) (BINOP(a)>>BINOP(b))
#define luai_numbsl(a, b) (BINOP(a)<<BINOP(b))
#define luai_numbnot(a) (~BINOP(a))

inline double luai_veclen(const float* a)
{
    double ps=0;
#if LUA_VECTOR_SIZE == 4
    if (a[3]==a[3]) ps+=(a[3]*a[3]);
#endif
    if (a[2]==a[2]) ps+=(a[2]*a[2]);
    ps+=(a[1]*a[1]);
    ps+=(a[0]*a[0]);
    return sqrt(ps);
}

inline bool luai_coleq(const unsigned char* a, const unsigned char* b)
{
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
}

inline bool luai_veceq(const float* a, const float* b)
{
#if LUA_VECTOR_SIZE == 4
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
#else
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
#endif
}

inline bool luai_vecisnan(const float* a)
{
#if LUA_VECTOR_SIZE == 4
    return a[0] != a[0] || a[1] != a[1] || a[2] != a[2] || a[3] != a[3];
#else
    return a[0] != a[0] || a[1] != a[1] || a[2] != a[2];
#endif
}

inline float luaui_signf(float v)
{
    return v > 0.0f ? 1.0f : v < 0.0f ? -1.0f : 0.0f;
}

inline float luaui_clampf(float v, float min, float max)
{
    float r = v < min ? min : v;
    return r > max ? max : r;
}

LUAU_FASTMATH_BEGIN
inline double luai_nummod(double a, double b)
{
    return a - floor(a / b) * b;
}
LUAU_FASTMATH_END

LUAU_FASTMATH_BEGIN
inline double luai_numidiv(double a, double b)
{
    return trunc(a / b);
}
LUAU_FASTMATH_END

inline float luai_lerpf(float a, float b, float t)
{
    return (t == 1.0) ? b : a + (b - a) * t;
}

#define luai_num2int(i, d) ((i) = (int)(d))

#define luai_num2unsigned(i, n) ((i) = (unsigned)(long long)(n))

#define LUAI_MAXNUM2STR 48

LUAI_FUNC char* luai_num2str(char* buf, double n);

#define luai_str2num(s, p) strtod((s), (p))
