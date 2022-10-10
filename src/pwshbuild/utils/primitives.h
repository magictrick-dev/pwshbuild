#ifndef PWSHBUILD_PRIMITIVES_H
#define PWSHBUILD_PRIMITIVES_H
#include <stdint.h>

/**
 * Provides primitive type information for integers, reals, and bools.
 */
typedef uint8_t			u8;
typedef uint16_t		u16;
typedef uint32_t		u32;
typedef uint64_t		u64;
typedef int8_t			i8;
typedef int16_t			i16;
typedef int32_t			i32;
typedef int64_t			i64;
typedef i32				b32;
typedef i64				b64;
typedef float			r32;
typedef double			r64;

#define BYTES(n)	 (size_t)(n)
#define KILOBYTES(n) (size_t)(BYTES(n)*1024)
#define MEGABYTES(n) (size_t)(KILOBYTES(n)*1024)
#define GIGABYTES(n) (size_t)(MEGABYTES(n)*1024)
#define TERABYTES(n) (size_t)(GIGABYTES(n)*1024)

/**
 * Since static qualifiers differ based on their useage, we are defining a series
 * of more explicit macros to differentiate them.
 */
#define persist static
#define global static
#define internal static

#endif