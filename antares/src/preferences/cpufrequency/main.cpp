/*
 * Copyright 2009, Antares, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Clemens Zeidler, antares@clemens-zeidler.de
 */

#include "PreferencesWindow.h"
#include "CPUFrequencyView.h"
#include <Application.h>
#include <Catalog.h>
#include <Locale.h>

#define TR_CONTEXT "Main window"

int 
main(int argc, char* argv[])
{
	BApplication	*app = new BApplication(kPrefSignature);

	BCatalog cat;
	be_locale->GetAppCatalog(&cat);

	PreferencesWindow<freq_preferences> *window;
	window = new PreferencesWindow<freq_preferences>(TR("CPU Frequency"),
														kPreferencesFileName,
														kDefaultPreferences);
	CPUFrequencyView* prefView = new CPUFrequencyView(BRect(0, 0, 400, 350),
														window);
	window->SetPreferencesView(prefView);
	window->Show();
	app->Run();
	
	delete app;
	return 0;
}
