/*
 * Copyright 2004-2007 Antares Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Author:
 *		Andre Alves Garzia, andre@andregarzia.com
 */
#ifndef SETTING_H
#define SETTING_H

#include <ObjectList.h>
#include <String.h>

class Setting {
	public:
		Setting(const char* name);
		virtual ~Setting();

		void SetName(BString name);
		void SetIP(BString ip) {fIP = ip; }
		void SetGateway(BString ip) {fGateway = ip; }
		void SetNetmask(BString ip) {fNetmask = ip; }
		void SetAutoConfigure(bool t) {fAuto = t; }
		
		const char* IP()  {return fIP.String(); }
		const char* Gateway()  {return fGateway.String(); }		
		const char* Netmask()  {return fNetmask.String(); }
		const char* Name()  {return fName.String(); }
		bool 		AutoConfigured() {return fAuto; }
		int			Media()		{ return fMedia; }
		BObjectList<BString> fNameservers;
		void ReadConfiguration();

		
	private:
		bool 	_PrepareRequest(struct ifreq& request);
		
		int 	fSocket;
		BString fIP;
		BString fGateway;
		BString fNetmask;
		BString fName;
		bool 	fAuto;
		int		fMedia;
};

#endif /* SETTINGS_H */
