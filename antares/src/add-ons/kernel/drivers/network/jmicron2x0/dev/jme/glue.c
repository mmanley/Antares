/*
 * Copyright 2009, Axel Dörfler. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */


#include <sys/bus.h>

ANTARES_FBSD_DRIVER_GLUE(jmicron2x0, jme, pci);

ANTARES_FBSD_MII_DRIVER(jmphy);
ANTARES_DRIVER_REQUIREMENTS(FBSD_TASKQUEUES | FBSD_SWI_TASKQUEUE | FBSD_FAST_TASKQUEUE);
NO_ANTARES_CHECK_DISABLE_INTERRUPTS();
NO_ANTARES_REENABLE_INTERRUPTS();
