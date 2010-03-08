/*
 * Copyright 2007, Ingo Weinhold, bonefish@cs.tu-berlin.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef _FSSH_COMPATIBILITY_H
#define _FSSH_COMPATIBILITY_H

#if (defined(__BEOS__) || defined(__ANTARES__))
#	ifndef ANTARES_HOST_PLATFORM_ANTARES
#		include <AntaresBuildCompatibility.h>
#	endif
#else
#	include <BeOSBuildCompatibility.h>
#endif

#endif	// _FSSH_COMPATIBILITY_H

