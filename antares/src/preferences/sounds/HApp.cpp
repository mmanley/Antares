/*
 * Copyright 2003-2006, Antares. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Atsushi Takamatsu
 *		Jérôme Duval
 *		Oliver Ruiz Dorantes
 */


#include "HApp.h"
#include "HWindow.h"

#include <Alert.h>


HApp::HApp()
	:
	BApplication("application/x-vnd.Antares-Sounds")
{
	BRect rect;
	rect.Set(200, 150, 590, 570);

	HWindow* window = new HWindow(rect, "Sounds");
	window->Show();
}


HApp::~HApp()
{
}


void
HApp::AboutRequested()
{
	BAlert* alert = new BAlert("About Sounds",
		"Sounds\n"
		"  Brought to you by :\n"
		"	Oliver Ruiz Dorantes\n"
		"	Jérôme DUVAL.\n"
		"  Original work from Atsushi Takamatsu.\n"
		"Copyright " B_UTF8_COPYRIGHT "2003-2006 Antares", "OK");
	alert->Go();
}


//	#pragma mark -


int
main(int, char**)
{
	HApp app;
	app.Run();

	return 0;
}

