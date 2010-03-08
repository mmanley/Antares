/*
 * Copyright 2009, Colin Günther, coling@gmx.de.
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */


#include <sys/bus.h>
#include <sys/kernel.h>

#include <machine/bus.h>

#include <net/if.h>
#include <net/if_media.h>

#include <net80211/ieee80211_var.h>

#include <dev/ipw/if_ipwreg.h>
#include <dev/ipw/if_ipwvar.h>


ANTARES_FBSD_WLAN_DRIVER_GLUE(iprowifi2100, ipw, pci)
NO_ANTARES_FBSD_MII_DRIVER();
NO_ANTARES_REENABLE_INTERRUPTS();
ANTARES_DRIVER_REQUIREMENTS(FBSD_TASKQUEUES | FBSD_SWI_TASKQUEUE | FBSD_WLAN);
ANTARES_FIRMWARE_VERSION(130);
ANTARES_FIRMWARE_NAME_MAP(3) = {{"ipw_bss", "ipw2100-1.3.fw"},
	{"ipw_ibss", "ipw2100-1.3-i.fw"}, {"ipw_monitor", "ipw2100-1.3-p.fw"}};


int
ANTARES_CHECK_DISABLE_INTERRUPTS(device_t dev)
{
	struct ipw_softc* sc = (struct ipw_softc*)device_get_softc(dev);
	uint32 r;

	r = CSR_READ_4(sc, IPW_CSR_INTR);
	if (r  == 0 || r == 0xffffffff)
		return 0;

	atomic_set((int32*)&sc->sc_intr_status, r);

	CSR_WRITE_4(sc, IPW_CSR_INTR_MASK, 0);
		// disable interrupts
	return 1;
}
