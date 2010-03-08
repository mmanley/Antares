/*
 * Copyright 2001-2006, Antares.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm <bpmagic@columbus.rr.com>
 *		Axel Dörfler, axeld@pinc-software.de
 *		Stephan Aßmus <superstippi@gmx.de>
 */
#ifndef REFERENCABLE_H
#define REFERENCABLE_H

#include <SupportDefs.h>

class Referenceable {
 public:
								Referenceable();
	virtual						~Referenceable();

			void				Acquire();
			bool				Release();

 private:
			vint32				fReferenceCount;
};

#endif // REFERENCABLE_H
