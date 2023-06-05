#ifndef __INC_TYPES_H__
#define __INC_TYPES_H__

#ifndef NULL
#define NULL ((void*) 0)
#endif

#define false	0x0
#define true 	0x1
// Represents true-or-false values
typedef _Bool bool;


// Explicitly-sized versions of integer types
typedef char				int8_t;
typedef unsigned char		uint8_t;
typedef short				int16_t;
typedef unsigned short		uint16_t;
typedef int					int32_t;
typedef unsigned int		uint32_t;
typedef long long			int64_t;
typedef unsigned long long	uint64_t;

// Pointers and addresses are 32 bits long.
// We use pointer types to represent virtual addresses,
// virtaddr_t to represent the numerical values of virtual addresses,
// and physaddr_t to represent physical addresses.
typedef int32_t intptr_t;
typedef uint32_t virtaddr_t;
typedef uint32_t physaddr_t;

typedef uint32_t size_t;
typedef int32_t ssize_t;
typedef int32_t off_t;

// Efficient min and max operations
#define MIN(_a, _b)						\
({								\
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a <= __b ? __a : __b;					\
})
#define MAX(_a, _b)						\
({								\
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a >= __b ? __a : __b;					\
})

// Rounding operations (efficient when n is a power of 2)
// Round down to the nearest multiple of n
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))

#endif 