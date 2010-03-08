/*
 * Copyright 2009, Antares, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _CONFIG_TYPES_H
#define _CONFIG_TYPES_H


#include <config/AntaresConfig.h>


/* fixed-width types -- the __antares_std_[u]int* types correspond to the POSIX
   [u]int*_t types, the _antares_[u]int* types to the BeOS [u]int* types. If
   __ANTARES_BEOS_COMPATIBLE_TYPES is not defined both sets are identical. Once
   we drop compatibility for good, we can consolidate the types.
*/
typedef signed char			__antares_std_int8;
typedef unsigned char		__antares_std_uint8;
typedef signed short		__antares_std_int16;
typedef unsigned short		__antares_std_uint16;
typedef signed int			__antares_std_int32;
typedef unsigned int		__antares_std_uint32;
typedef signed long long	__antares_std_int64;
typedef unsigned long long	__antares_std_uint64;

typedef __antares_std_int8	__antares_int8;
typedef __antares_std_uint8	__antares_uint8;
typedef __antares_std_int16	__antares_int16;
typedef __antares_std_uint16	__antares_uint16;
#ifdef __ANTARES_BEOS_COMPATIBLE_TYPES
typedef signed long int		__antares_int32;
typedef unsigned long int	__antares_uint32;
#else
typedef __antares_std_int32	__antares_int32;
typedef __antares_std_uint32	__antares_uint32;
#endif
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


/* printf()/scanf() format prefixes */
#define	__ANTARES_STD_PRI_PREFIX_32	""
#define	__ANTARES_STD_PRI_PREFIX_64	"ll"

#ifdef __ANTARES_BEOS_COMPATIBLE_TYPES
#	define	__ANTARES_PRI_PREFIX_32	"l"
#else
#	define	__ANTARES_PRI_PREFIX_32	__ANTARES_STD_PRI_PREFIX_32
#endif
#define	__ANTARES_PRI_PREFIX_64		__ANTARES_STD_PRI_PREFIX_64

#ifdef __ANTARES_ARCH_64_BIT
#	define __ANTARES_PRI_PREFIX_ADDR	__ANTARES_PRI_PREFIX_64
#else
#	define __ANTARES_PRI_PREFIX_ADDR	__ANTARES_PRI_PREFIX_32
#endif


#endif	/* _CONFIG_TYPES_H */
