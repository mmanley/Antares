/*
 * Copyright 2009, Ingo Weinhold, ingo_weinhold@gmx.de. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include <SupportDefs.h>

#include <image_defs.h>


// Antares API and ABI versions -- we compile those into shared objects so that
// the runtime loader can discriminate the versions and enable compatibility
// work-arounds, if necessary.

uint32 B_SHARED_OBJECT_ANTARES_VERSION_VARIABLE = B_ANTARES_VERSION;
uint32 B_SHARED_OBJECT_ANTARES_ABI_VARIABLE = B_ANTARES_ABI;
