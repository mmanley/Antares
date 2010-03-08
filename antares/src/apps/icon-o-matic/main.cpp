/*
 * Copyright 2006, Antares.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Stephan Aßmus <superstippi@gmx.de>
 */

#include "IconEditorApp.h"

int
main(int argc, char** argv)
{
	IconEditorApp* app = new IconEditorApp();
	app->Run();

	delete app;

	return 0;
}
