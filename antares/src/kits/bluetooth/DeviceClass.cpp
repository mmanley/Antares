/*
 * Copyright 2007-2008 Oliver Ruiz Dorantes, oliver.ruiz.dorantes_at_gmail.com
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <bluetooth/DeviceClass.h>

namespace Bluetooth {

void
DeviceClass::GetServiceClass(BString& serviceClass)
{
	static const char *services[] = { "Positioning", "Networking",
				"Rendering", "Capturing", "Object transfer",
				"Audio", "Telephony", "Information" };

	if (ServiceClass() != 0) {
		bool first = true;

		for (uint s = 0; s < (sizeof(services) / sizeof(*services)); s++) {
			if (ServiceClass() & (1 << s)) {
				if (first) {
					first = false;
					serviceClass << services[s];
				} else {
					serviceClass << ", " << services[s];
				}
					
			}
		}

	} else
		serviceClass << "Unspecified";

}


void
DeviceClass::GetMajorDeviceClass(BString& majorClass)
{
	static const char *major_devices[] = { "Miscellaneous", "Computer", "Phone",
				"LAN access", "Audio/Video", "Peripheral", "Imaging", "Uncategorized" };

	if (MajorDeviceClass() >= sizeof(major_devices) / sizeof(*major_devices))
		majorClass << "Invalid device class!\n";
	else
		majorClass << major_devices[MajorDeviceClass()];

}


void
DeviceClass::GetMinorDeviceClass(BString& minorClass)
{
	uint major = MajorDeviceClass();
	uint minor = MinorDeviceClass();
	
	switch (major) {
		case 0:	/* misc */
			minorClass << " -";
			break;
		case 1:	/* computer */
			switch(minor) {
				case 0:
					minorClass << "Uncategorized";
					break;	
				case 1:
					minorClass << "Desktop workstation";
					break;	
				case 2:
					minorClass << "Server";
					break;
				case 3:
					minorClass << "Laptop";
					break;
				case 4:
					minorClass << "Handheld";
					break;
				case 5:
					minorClass << "Palm";
					break;
				case 6:
					minorClass << "Wearable";
					break;
				}
			break;
		case 2:	/* phone */
			switch(minor) {
				case 0:
					minorClass << "Uncategorized";
					break;
				case 1:
					minorClass << "Cellular";
					break;
				case 2:
					minorClass << "Cordless";
					break;
				case 3:
					minorClass << "Smart phone";
					break;
				case 4:
					minorClass << "Wired modem or voice gateway";
					break;
				case 5:
					minorClass << "Common ISDN access";
					break;
				case 6:
					minorClass << "SIM card reader";
					break;
			}
			break;
		case 3:	/* lan access */
			if (minor == 0) {
				minorClass << "Uncategorized";
				break;
			}
			switch(minor / 8) {
				case 0:
					minorClass << "Fully available";
					break;
				case 1:
					minorClass << "1-17% utilized";
					break;
				case 2:
					minorClass << "17-33% utilized";
					break;
				case 3:
					minorClass << "33-50% utilized";
					break;
				case 4:
					minorClass << "50-67% utilized";
					break;
				case 5:
					minorClass << "67-83% utilized";
					break;
				case 6:
					minorClass << "83-99% utilized";
					break;
				case 7:
					minorClass << "No service available";
					break;
			}
			break;
		case 4:	/* audio/video */
			switch(minor) {
				case 0:
					minorClass << "Uncategorized";
					break;
				case 1:
					minorClass << "Device conforms to the headset profile";
					break;
				case 2:
					minorClass << "Hands-free";
					break;
					/* 3 is reserved */
				case 4:
					minorClass << "Microphone";
					break;
				case 5:
					minorClass << "Loudspeaker";
					break;
				case 6:
					minorClass << "Headphones";
					break;
				case 7:
					minorClass << "Portable audio";
					break;
				case 8:
					minorClass << "Car audio";
					break;
				case 9:
					minorClass << "Set-top box";
					break;
				case 10:
					minorClass << "HiFi audio device";
					break;
				case 11:
					minorClass << "VCR";
					break;
				case 12:
					minorClass << "Video camera";
					break;
				case 13:
					minorClass << "Camcorder";
					break;
				case 14:
					minorClass << "Video monitor";
					break;
				case 15:
					minorClass << "Video display and loudspeaker";
					break;
				case 16:
					minorClass << "Video conferencing";
					break;
					/* 17 is reserved */
				case 18:
					minorClass << "Gaming/Toy";
					break;
			}
			break;
		case 5:	/* peripheral */
		{		
			switch(minor & 48) {
				case 16:
					minorClass << "Keyboard";
					if (minor & 15)
						minorClass << "/";
					break;
				case 32:
					minorClass << "Pointing device";
					if (minor & 15)
						minorClass << "/";
					break;
				case 48:
					minorClass << "Combo keyboard/pointing device";
					if (minor & 15)
						minorClass << "/";							
					break;
			}
			
			switch(minor & 15) {
				case 0:
					break;
				case 1:
					minorClass << "Joystick";
					break;
				case 2:
					minorClass << "Gamepad";
					break;
				case 3:
					minorClass << "Remote control";
					break;
				case 4:
					minorClass << "Sensing device";
					break;
				case 5:
					minorClass << "Digitizer tablet";
					break;
				case 6:
					minorClass << "Card reader";
					break;
				default:
					minorClass << "(reserved)";
					break;
			}
			break;
		}
		case 6:	/* imaging */
			if (minor & 4)
				minorClass << "Display";
			if (minor & 8)
				minorClass << "Camera";
			if (minor & 16)
				minorClass << "Scanner";
			if (minor & 32)
				minorClass << "Printer";
			break;
		case 7: /* wearable */
			switch(minor) {
				case 1:
					minorClass << "Wrist watch";
					break;
				case 2:
					minorClass << "Pager";
					break;
				case 3:
					minorClass << "Jacket";
					break;
				case 4:
					minorClass << "Helmet";
					break;
				case 5:
					minorClass << "Glasses";
					break;
			}
			break;
		case 8: /* toy */
			switch(minor) {
				case 1:
					minorClass << "Robot";
					break;	
				case 2:
					minorClass << "Vehicle";
					break;
				case 3:
					minorClass << "Doll/Action figure";
					break;
				case 4:
					minorClass << "Controller";
					break;
				case 5:
					minorClass << "Game";
					break;
			}
			break;
		case 63:	/* uncategorised */
			minorClass << "";
			break;
		default:
			minorClass << "Unknown (reserved) minor device class";
			break;
	}
}


void
DeviceClass::DumpDeviceClass(BString& string)
{
	string << "Service classes: ";
	GetServiceClass(string);
	string << " | ";
	string << "Major class: ";
	GetMajorDeviceClass(string);
	string << " | ";
	string << "Minor class: ";
	GetMinorDeviceClass(string);
	string << ".";
}


void
DeviceClass::Draw(BView* view, const BPoint& point)
{
	rgb_color	kBlack = { 0,0,0,0 };
	rgb_color	kBlue = { 28,110,157,0 };
	rgb_color	kWhite = { 255,255,255,0 };


	view->SetHighColor(kBlue);
	view->FillRoundRect(BRect(point.x + IconInsets, point.y + IconInsets, 
		point.x + IconInsets + PixelsForIcon, point.y + IconInsets + PixelsForIcon), 5, 5);

	view->SetHighColor(kWhite);

	switch (MajorDeviceClass()) {

		case 2: // phone
			view->StrokeRoundRect(BRect(point.x + IconInsets + uint(PixelsForIcon/4),
				 point.y + IconInsets + 6,
				 point.x + IconInsets + uint(PixelsForIcon*3/4),
			 	 point.y + IconInsets + PixelsForIcon - 2), 2, 2);
			view->StrokeRect(BRect(point.x + IconInsets + uint(PixelsForIcon/4) + 4,
			 	 point.y + IconInsets + 10,
				 point.x + IconInsets + uint(PixelsForIcon*3/4) - 4,
			 	 point.y + IconInsets + uint(PixelsForIcon*3/4)));
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon/4) + 4,
				 point.y + IconInsets + PixelsForIcon - 6), 
				 BPoint(point.x + IconInsets + uint(PixelsForIcon*3/4) - 4,
				 point.y + IconInsets + PixelsForIcon - 6));
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon/4) + 4,
				 point.y + IconInsets + PixelsForIcon - 4), 
				 BPoint(point.x + IconInsets + uint(PixelsForIcon*3/4) - 4,
				 point.y + IconInsets + PixelsForIcon - 4));
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon/4) + 4,
				 point.y + IconInsets + 2), 
				 BPoint(point.x + IconInsets + uint(PixelsForIcon/4) + 4,
				 point.y + IconInsets + 6));
			break;
		case 3: // LAN
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon/4),
				 point.y + IconInsets + uint(PixelsForIcon*3/8)),
				BPoint(point.x + IconInsets + uint(PixelsForIcon*3/4),
				 point.y + IconInsets + uint(PixelsForIcon*3/8)));
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon*5/8),
				 point.y + IconInsets + uint(PixelsForIcon/8)));			
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon*3/4),
				 point.y + IconInsets + uint(PixelsForIcon*5/8)),
				BPoint(point.x + IconInsets + uint(PixelsForIcon/4),
				 point.y + IconInsets + uint(PixelsForIcon*5/8)));
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon*3/8),
				 point.y + IconInsets + uint(PixelsForIcon*7/8)));
			break;
		case 4: // audio/video
			view->StrokeRect(BRect(point.x + IconInsets + uint(PixelsForIcon/4),
				 point.y + IconInsets + uint(PixelsForIcon*3/8),
				 point.x + IconInsets + uint(PixelsForIcon*3/8),
			 	 point.y + IconInsets + uint(PixelsForIcon*5/8)));
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon*3/8),
				 point.y + IconInsets + uint(PixelsForIcon*3/8)),
				BPoint(point.x + IconInsets + uint(PixelsForIcon*3/4),
				 point.y + IconInsets + uint(PixelsForIcon/8)));
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon*3/4),
				 point.y + IconInsets + uint(PixelsForIcon*7/8)));
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon*3/8),
				 point.y + IconInsets + uint(PixelsForIcon*5/8)));
			break;			
		default: // Bluetooth Logo
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon/4),
				 point.y + IconInsets + uint(PixelsForIcon*3/4)),
				BPoint(point.x + IconInsets + uint(PixelsForIcon*3/4),
				 point.y + IconInsets + uint(PixelsForIcon/4)));
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon/2),
				 point.y + IconInsets +2));
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon/2),
				 point.y + IconInsets + PixelsForIcon - 2));
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon*3/4),
				 point.y + IconInsets + uint(PixelsForIcon*3/4)));
			view->StrokeLine(BPoint(point.x + IconInsets + uint(PixelsForIcon/4), 
				point.y + IconInsets + uint(PixelsForIcon/4)));
			break;
	}
	
	view->SetHighColor(kBlack);
}


}