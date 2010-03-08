/*
 * Copyright 2009, Antares, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _CONFIG_BUILD_TYPES_H
#define _CONFIG_BUILD_TYPES_H


#include <config_build/AntaresConfig.h>

#include <inttypes.h>


/* fixed-width types -- the __antares_std_[u]int* types correspond to the POSIX
   [u]int*_t types, the _antares_[u]int* types to the BeOS [u]int* types. If
   __ANTARES_BEOS_COMPATIBLE_TYPES is not defined both sets are identical. Once
   we drop compatibility for good, we can consolidate the types.
*/
typedef int8_t				__antares_std_int8;
typedef uint8_t				__antares_std_uint8;
typedef int16_t				__antares_std_int16;
typedef uint16_t			__antares_std_uint16;
typedef int32_t				__antares_std_int32;
typedef uint32_t			__antares_std_uint32;
typedef int64_t				__antares_std_int64;
typedef uint64_t			__antares_std_uint64;

typedef __antares_std_int8	__antares_int8;
typedef __antares_std_uint8	__antares_uint8;
typedef __antares_std_int16	__antares_int16;
typedef __antares_std_uint16	__antares_uint16;
typedef __antares_std_int32	__antares_int32;
typedef __antares_std_uint32	__antares_uint32;
typedef __antares_std_int64	__antares_int64;
typedef __antares_std_uint64	__antares_uint64;

/* address types */
#ifdef __ANTARES_ARCH_64_BIT
	typedef	__antares_int64	__antares_saddr_t;
	typedef	__antares_uint64	__antares_addr_t;
#else
	typedef	__antares_int32	__antares_saddr_t;
	typedef	__antares_uint32	__antares_addr_t;
#endif

/* address type limits */
#ifdef __ANTARES_ARCH_64_BIT
#	define __ANTARES_SADDR_MAX	(9223372036854775807LL)
#	define __ANTARES_ADDR_MAX		(18446744073709551615ULL)
#else
#	define __ANTARES_SADDR_MAX	(2147483647)
#	define __ANTARES_ADDR_MAX		(4294967295U)
#endif
#define __ANTARES_SADDR_MIN		(-__ANTARES_SADDR_MAX-1)


#endif	/* _CONFIG_BUILD_TYPES_H */
