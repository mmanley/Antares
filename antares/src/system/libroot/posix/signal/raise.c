/*
 *  Copyright (c) 2002-2006, Antares Project. All rights reserved.
 *  Distributed under the terms of the Antares license.
 *
 *  Author(s):
 *  Daniel Reinhold (danielre@users.sf.net)
 */


#include <OS.h>
#include <signal.h>


int
raise(int sig)
{
	return kill(find_thread(NULL), sig);
}

