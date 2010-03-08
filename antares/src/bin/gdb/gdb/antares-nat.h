/* Antares native-dependent definitions.

   Copyright 2005 Ingo Weinhold <bonefish@cs.tu-berlin.de>.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef ANTARES_NAT_H
#define ANTARES_NAT_H

#include <debugger.h>

/* Required by antares-nat.c, implemented in <arch>-antares-nat.c. */

void antares_supply_registers(int reg, const debug_cpu_state *cpuState);
void antares_collect_registers(int reg, debug_cpu_state *cpuState);


struct antares_image_info;

/* Function used by solib-antares.c to iterate through the list of images of
   the inferior.
   TODO: This must go, since it works only in a native debugger. We can
   probably tunnel these data through the xfer_memory() function.
*/
struct antares_image_info *antares_get_next_image_info(int lastID);

#endif	/* ANTARES_NAT_H */
