/*
 * Copyright 2009, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef ELF_ANTARES_VERSION_H
#define ELF_ANTARES_VERSION_H

#include "runtime_loader_private.h"


void	analyze_image_antares_version_and_abi(int fd, image_t* image,
			Elf32_Ehdr& eheader, int32 sheaderSize, char* buffer,
			size_t bufferSize);


#endif	// ELF_ANTARES_VERSION_H