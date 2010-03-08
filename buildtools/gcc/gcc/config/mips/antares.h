/* Definitions for MIPS running Antares
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */


#undef TARGET_VERSION
#define TARGET_VERSION fprintf (stderr, " (MIPSEL Antares/ELF)");

#define TARGET_OS_CPP_BUILTINS()					\
  do									\
    {									\
	builtin_define ("__ANTARES__");					\
	builtin_define ("__MIPS__");					\
	builtin_define ("__MIPSEL__");					\
	builtin_define ("_MIPSEL_");					\
	builtin_define ("__stdcall=__attribute__((__stdcall__))");	\
	builtin_define ("__cdecl=__attribute__((__cdecl__))");		\
	builtin_assert ("system=antares");					\
	if (flag_pic)							\
	  {								\
	    builtin_define ("__PIC__");					\
	    builtin_define ("__pic__");					\
	  }								\
    /* Antares apparently doesn't support merging of symbols across shared \
       object boundaries. Hence we need to explicitly specify that \
       type_infos are not merged, so that they get compared by name \
       instead of by pointer. */ \
    	builtin_define ("__GXX_MERGED_TYPEINFO_NAMES=0");		\
	builtin_define ("__GXX_TYPEINFO_EQUALITY_INLINE=0"); 		\
    }									\
  while (0)

#undef	LINK_SPEC
#define LINK_SPEC "%{!o*:-o %b} -m elf_mipsel_antares -shared -Bsymbolic %{nostart:-e 0}"

