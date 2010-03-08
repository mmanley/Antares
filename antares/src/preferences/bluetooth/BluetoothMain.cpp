/*
 * Copyright 2008-10, Oliver Ruiz Dorantes, <oliver.ruiz.dorantes_at_gmail.com>
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#include <stdio.h>

#include <Alert.h>
#include <Catalog.h>
#include <Locale.h>
#include <MessageRunner.h>
#include <Roster.h>

#include "BluetoothMain.h"
#include "BluetoothWindow.h"
#include "defs.h"


#define TR_CONTEXT "main"

BluetoothApplication::BluetoothApplication(void)
 :	BApplication(BLUETOOTH_APP_SIGNATURE)
{
	be_locale->GetAppCatalog(&fCatalog);
}


void
BluetoothApplication::ReadyToRun()
{
	if (!be_roster->IsRunning(BLUETOOTH_SIGNATURE)) {

		int32 choice = (new BAlert("bluetooth_server not running",
			"bluetooth_server has not been found running on the system."
			"Should be started, or stay offline", "Work offline",
			"Quit", "Start please", B_WIDTH_AS_USUAL, B_WARNING_ALERT))->Go();


		switch (choice) {
			case 1:
				PostMessage(B_QUIT_REQUESTED);
				return;
			case 2:
			{
				status_t error;
				error = be_roster->Launch(BLUETOOTH_SIGNATURE);
				printf("kMsgStartServices: %s\n", strerror(error));
				// TODO: This is temporal
				// BMessage handcheck: use the version of Launch()
				// that includes a BMessage	in that message include
				// a BMessenger to yourself and the BT server could
				// use that messenger to send back a reply indicating
				// when it's ready and you could just create window
				// when you get that
				BMessageRunner::StartSending(be_app_messenger,
					new BMessage('Xtmp'), 2 * 1000000, 1);

				break;
			}
			default:
				PostMessage(new BMessage('Xtmp'));
				break;
		}
	}

}


void
BluetoothApplication::AboutRequested()
{

	(new BAlert("about", TR("Antares Bluetooth system, (ARCE)\n\n"
							"Created by Oliver Ruiz Dorantes\n\n"
							"With support of:\n"
							"	- Mika Lindqvist\n"
							"	- Adrien Destugues\n"
							"	- Maksym Yevmenkin\n\n"
							"Thanks to the individuals who helped...\n\n"
							"Shipping/donating hardware:\n"
							"	- Henry Jair Abril Florez (el Colombian)\n"
							"		& Stefanie Bartolich\n"
							"	- Edwin Erik Amsler\n"
							"	- Dennis d'Entremont\n"
							"	- Luroh\n"
							"	- Pieter Panman\n\n"
							"Economically:\n"
							"	- Karl vom Dorff, Andrea Bernardi (OSDrawer),\n"
							"	- Matt M, Doug F, Hubert H,\n"
							"	- Sebastian B, Andrew M, Jared E,\n"
							"	- Frederik H, Tom S, Ferry B,\n"
							"	- Greg G, David F, Richard S, Martin W:\n\n"
							"With patches:\n"
							"	- Michael Weirauch\n"
							"	- Fredrik Ekdahl\n"
							"	- Raynald Lesieur\n"
							"	- Andreas Färber\n"
							"	- Joerg Meyer\n"
							"Testing:\n"
							"	- Petter H. Juliussen\n"
							"Who gave me all the knowledge:\n"
							"	- the yellowTAB team"), TR("OK")))->Go();

}


void
BluetoothApplication::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgAddToRemoteList:
			fWindow->PostMessage(message);
			break;
		case 'Xtmp':
			fWindow = new BluetoothWindow(BRect(100, 100, 550, 420));
			fWindow->Show();
			break;
		default:
			BApplication::MessageReceived(message);
	}
}


int
main(int, char**)
{
	BluetoothApplication myApplication;
	myApplication.Run();

	return 0;
}