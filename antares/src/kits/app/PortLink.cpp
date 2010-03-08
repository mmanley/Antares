/*
 * Copyright 2005-2009, Antares Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Axel Dörfler, axeld@pinc-software.de
 */


#include <PortLink.h>


namespace BPrivate {


PortLink::PortLink(port_id send, port_id receive)
{
	fSender = new LinkSender(send);
	fReceiver = new LinkReceiver(receive);
}


PortLink::~PortLink()
{
	delete fReceiver;
	delete fSender;
}


}	// namespace BPrivate