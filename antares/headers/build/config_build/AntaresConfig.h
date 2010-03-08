/*
 * Copyright 2009, Antares, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _CONFIG_ANTARES_CONFIG_H
#define _CONFIG_ANTARES_CONFIG_H


/* Determine the architecture and define macros for some fundamental
   properties:
   __ANTARES_ARCH			- short name of the architecture (used in paths)
   __ANTARES_ARCH_<arch>	- defined to 1 for the respective architecture
   __ANTARES_ARCH_64_BIT	- defined to 1 on 64 bit architectures
   __ANTARES_BIG_ENDIAN	- defined to 1 on big endian architectures
*/
#ifdef __INTEL__
#	ifdef ANTARES_HOST_PLATFORM_64_BIT
#		define __ANTARES_ARCH			x86_64
#		define __ANTARES_ARCH_X86_64	1
#		define __ANTARES_ARCH_64_BIT	1
#	else
#		define __ANTARES_ARCH			x86
#		define __ANTARES_ARCH_X86		1
#	endif
#elif __POWERPC__
#	define __ANTARES_ARCH				ppc
#	define __ANTARES_ARCH_PPC			1
#	define __ANTARES_BIG_ENDIAN		1
#elif __M68K__
#	define __ANTARES_ARCH				m68k
#	define __ANTARES_ARCH_M68K		1
#	define __ANTARES_BIG_ENDIAN		1
#elif __MIPSEL__
#	define __ANTARES_ARCH				mipsel
#	define __ANTARES_ARCH_MIPSEL		1
#elif __ARM__
#	define __ANTARES_ARCH				arm
#	define __ANTARES_ARCH_ARM			1
#else
#	error Unsupported architecture!
#endif

/* implied properties */
#ifndef __ANTARES_ARCH_64_BIT
#	define	__ANTARES_ARCH_32_BIT		1
#endif
#ifndef __ANTARES_BIG_ENDIAN
#	define	__ANTARES_LITTLE_ENDIAN	1
#endif

/* architecture specific include macros */
#define __ANTARES_ARCH_HEADER(header)					<arch/__ANTARES_ARCH/header>
#define __ANTARES_SUBDIR_ARCH_HEADER(subdir, header)	\
	<subdir/arch/__ANTARES_ARCH/header>

#endif	/* _CONFIG_ANTARES_CONFIG_H */
