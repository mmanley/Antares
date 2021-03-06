/*
 * Copyright 2007-2009, Antares, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _BE_BUILD_H
#define _BE_BUILD_H


#include <config/AntaresConfig.h>


#define B_BEOS_VERSION_4				0x0400
#define B_BEOS_VERSION_4_5				0x0450
#define B_BEOS_VERSION_5				0x0500

#define B_BEOS_VERSION					B_BEOS_VERSION_5
#define B_BEOS_VERSION_MAUI				B_BEOS_VERSION_5

/* Antares (API) version */
#define B_ANTARES_VERSION_BEOS			0x00000001
#define B_ANTARES_VERSION_BONE			0x00000002
#define B_ANTARES_VERSION_DANO			0x00000003
#define B_ANTARES_VERSION_1_ALPHA_1		0x00000100
#define B_ANTARES_VERSION_1_PRE_ALPHA_2	0x00000101
#define B_ANTARES_VERSION_1				0x00010000

#define B_ANTARES_VERSION					B_ANTARES_VERSION_1_PRE_ALPHA_2

/* Antares ABI */
#define B_ANTARES_ABI_MAJOR				0xffff0000
#define B_ANTARES_ABI_GCC_2				0x00020000
#define B_ANTARES_ABI_GCC_4				0x00040000

#define B_ANTARES_ABI_GCC_2_ANCIENT		0x00020000
#define B_ANTARES_ABI_GCC_2_BEOS			0x00020001
#define B_ANTARES_ABI_GCC_2_ANTARES			0x00020002

#if __GNUC__ == 2
#	define B_ANTARES_ABI					B_ANTARES_ABI_GCC_2_ANTARES
#elif __GNUC__ == 4
#	define B_ANTARES_ABI					B_ANTARES_ABI_GCC_4
#else
#	error Unsupported gcc version!
#endif


#ifdef __ANTARES_ARCH_64_BIT
#	define B_ANTARES_64_BIT				1
#else
#	define B_ANTARES_32_BIT				1
#endif

#ifdef __ANTARES_BEOS_COMPATIBLE
#	define B_ANTARES_BEOS_COMPATIBLE		1
#endif


#define _UNUSED(argument) argument
#define _PACKED __attribute__((packed))
#define _PRINTFLIKE(_format_, _args_) \
	__attribute__((format(__printf__, _format_, _args_)))
#define _EXPORT
#define _IMPORT

#define B_DEFINE_SYMBOL_VERSION(function, versionedSymbol)	\
	__asm__(".symver " function "," versionedSymbol)

#endif	/* _BE_BUILD_H */
