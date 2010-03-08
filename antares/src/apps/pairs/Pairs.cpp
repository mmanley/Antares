/*
 * Copyright 2008 Ralf Schülke, ralf.schuelke@googlemail.com. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include <stdlib.h>

#include <Application.h>
#include <Catalog.h>
#include <Locale.h>

#include "Pairs.h"
#include "PairsWindow.h"

const char* kSignature = "application/x-vnd.Antares-Pairs";


Pairs::Pairs()
	: BApplication(kSignature),
	  fWindow(NULL)
{
	be_locale->GetAppCatalog(&fCatalog);
}


Pairs::~Pairs()
{
}


void
Pairs::ReadyToRun()
{
	fWindow = new PairsWindow();
	fWindow->Show();
}


void
Pairs::RefsReceived(BMessage* message)
{
	fWindow->PostMessage(message);
}


void
Pairs::MessageReceived(BMessage* message)
{
	BApplication::MessageReceived(message);
}


int
main(void)
{
	Pairs pairs;
	pairs.Run();

	return 0;
}