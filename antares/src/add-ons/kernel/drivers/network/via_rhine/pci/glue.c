/*
 * Copyright 2007, Axel Dörfler, axeld@pinc-software.de. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */


#include <sys/bus.h>


ANTARES_FBSD_DRIVER_GLUE(via_rhine, vr, pci);
ANTARES_DRIVER_REQUIREMENTS(FBSD_TASKQUEUES | FBSD_SWI_TASKQUEUE);


extern driver_t *DRIVER_MODULE_NAME(ciphy, miibus);
extern driver_t *DRIVER_MODULE_NAME(ukphy, miibus);


driver_t *
__antares_select_miibus_driver(device_t dev)
{
	driver_t *drivers[] = {
		DRIVER_MODULE_NAME(ciphy, miibus),
		DRIVER_MODULE_NAME(ukphy, miibus),
		NULL
	};

	return __antares_probe_miibus(dev, drivers);
}

