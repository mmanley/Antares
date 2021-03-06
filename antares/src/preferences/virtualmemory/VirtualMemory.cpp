/*
 * Copyright 2005, Axel Dörfler, axeld@pinc-software.de. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include "VirtualMemory.h"
#include "SettingsWindow.h"

#include <Alert.h>
#include <TextView.h>


#undef TR_CONTEXT
#define TR_CONTEXT "VirtualMemoryApp"


VirtualMemory::VirtualMemory()
	: BApplication("application/x-vnd.Antares-VirtualMemory")
{
	be_locale->GetAppCatalog(&fCatalog);
}


VirtualMemory::~VirtualMemory()
{
}


void
VirtualMemory::ReadyToRun()
{
	BWindow* window = new SettingsWindow();
	window->Show();
}


void
VirtualMemory::AboutRequested()
{
	BAlert* alert = new BAlert("about", TR("VirtualMemory\n"
		"\twritten by Axel Dörfler\n"
		"\tCopyright 2005, Antares.\n"), TR("OK"));
	BTextView* view = alert->TextView();
	BFont font;

	view->SetStylable(true);

	view->GetFont(&font);
	font.SetSize(18);
	font.SetFace(B_BOLD_FACE); 			
	view->SetFontAndColor(0, 13, &font);

	alert->Go();
}


int
main(int argc, char** argv)
{
	VirtualMemory app;
	app.Run();

	return 0;
}
