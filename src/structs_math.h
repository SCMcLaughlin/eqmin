
#ifndef STRUCTS_MATH_H
#define STRUCTS_MATH_H

#include "define.h"

#define TYPE_Vec2(typename, id) \
typedef struct Vec2##id {       \
    union {                     \
        struct {                \
            typename x, y;      \
        };                      \
        struct {                \
            typename width;     \
            typename height;    \
        };                      \
        struct {                \
            typename u, v;      \
        };                      \
        struct {                \
            typename r, s;      \
        };                      \
        typename array[2];      \
    };                          \
} Vec2##id

TYPE_Vec2(float, f);
TYPE_Vec2(int, i);
TYPE_Vec2(uint32_t, u);

#define TYPE_Vec3(typename, id) \
typedef struct Vec3##id {       \
    union {                     \
        struct {                \
            typename x, y, z;   \
        };                      \
        struct {                \
            typename u, v, w;   \
        };                      \
        struct {                \
            typename r, g, b;   \
        };                      \
        typename array[3];      \
    };                          \
} Vec3##id

TYPE_Vec3(float, f);
TYPE_Vec3(int, i);
TYPE_Vec3(uint32_t, u);

#define TYPE_Vec4(typename, id)     \
typedef struct Vec4##id {           \
    union {                         \
        struct {                    \
            typename x, y, z, w;    \
        };                          \
        struct {                    \
            typename r, g, b, a;    \
        };                          \
        typename array[4];          \
    };                              \
} Vec4##id

TYPE_Vec4(float, f);
TYPE_Vec4(int, i);
TYPE_Vec4(uint32_t, u);
TYPE_Vec4(uint8_t, u8);

#undef TYPE_Vec2
#undef TYPE_Vec3
#undef TYPE_Vec4

typedef struct Triangle {
    Vec3f   pos[3];
} Triangle;

#endif/*STRUCTS_MATH_H*/
