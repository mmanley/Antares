/*
 * Copyright (c) 2005-2008, Antares, Inc.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *		DarkWyrm <darkwyrm@earthlink.net>
 */
#ifndef RESWIN_H
#define RESWIN_H

#include <Window.h>


struct entry_ref;

class ResWindow : public BWindow {
public:
								ResWindow(const BRect& rect,
									const entry_ref* ref = NULL);
	virtual						~ResWindow();

	virtual	bool				QuitRequested();
};

#endif // RESWIN_H