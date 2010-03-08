/*
 * Copyright 2001-2009, Antares.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ingo Weinhold (bonefish@users.sf.net)
 */


//! API classes - registrar interface.


#include <AppMisc.h>
#include <RegistrarDefs.h>


namespace BPrivate {

// names
#ifdef ANTARES_TARGET_PLATFORM_ANTARES
const char* kRegistrarSignature = "application/x-vnd.antares-registrar";
const char* kRAppLooperPortName = "rAppLooperPort";
#else
const char* kRegistrarSignature = "application/x-vnd.test-registrar";
const char* kRAppLooperPortName = "antares-test:rAppLooperPort";
#endif
const char* kRosterThreadName = "_roster_thread_";


/*!	\brief Returns the name of the main request port of the registrar (roster).
	\return the name of the registrar request port.
*/
const char*
get_roster_port_name()
{
#ifdef ANTARES_TARGET_PLATFORM_ANTARES
	return "system:roster";
#else
	return "antares-test:roster";
#endif
}

}	// namespace BPrivate
