/*
 * Copyright (c) 2008-2009, Antares, Inc.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *		Artur Wyszynski <harakash@gmail.com>
 */


#include "GradientsApp.h"


#include "GradientsWindow.h"


GradientsApp::GradientsApp(void)
	: BApplication("application/x-vnd.Antares-Gradients")
{
	GradientsWindow* window = new GradientsWindow();
	window->Show();
}


int
main()
{
	GradientsApp app;
	app.Run();
	return 0;
}
