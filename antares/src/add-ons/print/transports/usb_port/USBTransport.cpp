/*****************************************************************************/
// USB port transport add-on,
// changes by Andreas Benzler, Philippe Houdoin
//
// Original from Parallel 
// port transport add-on.
//
// Author
//   Michael Pfeiffer
// 
// This application and all source files used in its construction, except 
// where noted, are licensed under the MIT License, and have been written 
// and are:
//
// Copyright (c) 2001-2004 OpenBeOS Project
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
/*****************************************************************************/


#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <StorageKit.h>
#include <SupportKit.h>

#include <USB_printer.h>

#include "PrintTransportAddOn.h"


class USBTransport : public BDataIO 
{	
public:
	USBTransport(BDirectory *printer, BMessage *msg);
	~USBTransport();

	status_t InitCheck() { return fFile > -1 ? B_OK : B_ERROR; };

	ssize_t Read(void *buffer, size_t size);
	ssize_t Write(const void *buffer, size_t size);

private:
	int fFile;
};


// Implementation of transport add-on interface

BDataIO * 
instantiate_transport(BDirectory *printer, BMessage *msg) 
{
	USBTransport * transport = new USBTransport(printer, msg);
	if (transport->InitCheck() == B_OK)
		return transport;
	
	delete transport; 
	return NULL;
}

status_t list_transport_ports(BMessage* msg)
{
	BDirectory dir("/dev/printer/usb");
	status_t rc;

	if ((rc=dir.InitCheck()) != B_OK)
		return rc;

	if ((rc=dir.Rewind()) != B_OK)
		return rc;

	entry_ref ref;
	while(dir.GetNextRef(&ref) == B_OK)
		msg->AddString("port_id", ref.name);

	return B_OK;
}


// Implementation of USBTransport

USBTransport::USBTransport(BDirectory *printer, BMessage *msg) 
	: fFile(-1)
{
	char device_id[USB_PRINTER_DEVICE_ID_LENGTH + 1];
	char name[USB_PRINTER_DEVICE_ID_LENGTH + 1];
	char *desc;
	char *value;
	int ret;
	bool bidirectional = true;
	char *next_token;
	
	// We support only one USB printer, so does BeOS R5.
	fFile = open("/dev/printer/usb/0", O_RDWR | O_EXCL | O_BINARY, 0);
	if (fFile < 0) {
		// Try unidirectional access mode
		bidirectional = false;
		fFile = open("/dev/printer/usb/0", O_WRONLY | O_EXCL | O_BINARY, 0);
	}
	
	if (fFile < 0)
		return;
	
	// Get printer's DEVICE ID string
	ret = ioctl(fFile, USB_PRINTER_GET_DEVICE_ID, device_id, sizeof(device_id));
	if (ret < 0) {
		close(fFile);
		fFile = -1;
		return;
	}
	
	if (! msg)
		// Caller don't care about transport init message output content...
		return;
	
	// Fill up the message
	msg->what = 'okok';
	
	msg->AddBool("bidirectional", bidirectional);
	msg->AddString("device_id", device_id);

	// parse and split the device_id string into separate parameters
	desc = strtok_r(device_id, ":", &next_token);	
	while (desc) {
		snprintf(name, sizeof(name), "DEVID:%s", desc);
		value = strtok_r(NULL, ";", &next_token);
		if (!value)
			break;
		msg->AddString(name, value);
		
		// next device descriptor
		desc = strtok_r(NULL, ":", &next_token);	
	}
}


USBTransport::~USBTransport()
{
	if (fFile > -1)
		close(fFile);
}


ssize_t
USBTransport::Read(void *buffer, size_t size)
{
	return read(fFile, buffer, size);
}


ssize_t
USBTransport::Write(const void *buffer, size_t size)
{
	return write(fFile, buffer, size);
}

