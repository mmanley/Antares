/*
 * Copyright 2007, Hugo Santos. All Rights Reserved.
 * Copyright 2007, Axel Dörfler, axeld@pinc-software.de. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */


#include <sys/bus.h>


ANTARES_FBSD_DRIVER_GLUE(ar81xx, ale, pci)

extern driver_t *DRIVER_MODULE_NAME(ukphy, miibus);

driver_t *
__antares_select_miibus_driver(device_t dev)
{
	driver_t *drivers[] = {
		DRIVER_MODULE_NAME(ukphy, miibus),
		NULL
	};

	return __antares_probe_miibus(dev, drivers);
}

NO_ANTARES_CHECK_DISABLE_INTERRUPTS();
NO_ANTARES_REENABLE_INTERRUPTS();

ANTARES_DRIVER_REQUIREMENTS(FBSD_TASKQUEUES | FBSD_FAST_TASKQUEUE | FBSD_SWI_TASKQUEUE);
