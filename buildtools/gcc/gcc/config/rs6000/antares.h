/* Definitions for PowerPC running Antares
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004, 2005
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
#define TARGET_VERSION fprintf (stderr, " (PowerPC Antares/ELF)");

/* long double is 128 bits wide; the documentation claims
   LIBGCC2_LONG_DOUBLE_TYPE_SIZE to default to LONG_DOUBLE_TYPE_SIZE, but
   it apparently does not */
/*#undef LONG_DOUBLE_TYPE_SIZE
#define LONG_DOUBLE_TYPE_SIZE 128
#undef LIBGCC2_LONG_DOUBLE_TYPE_SIZE
#define LIBGCC2_LONG_DOUBLE_TYPE_SIZE 128*/

#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()					\
  do									\
    {									\
	builtin_define ("__ANTARES__");					\
	builtin_define ("__POWERPC__");					\
	builtin_define ("__powerpc__");					\
	builtin_define ("__stdcall=__attribute__((__stdcall__))");	\
	builtin_define ("__cdecl=__attribute__((__cdecl__))");		\
	builtin_assert ("system=antares");					\
	builtin_assert ("cpu=powerpc");					\
	builtin_assert ("machine=powerpc");					\
	TARGET_OS_SYSV_CPP_BUILTINS ();					\
    /* Antares apparently doesn't support merging of symbols across shared \
       object boundaries. Hence we need to explicitly specify that \
       type_infos are not merged, so that they get compared by name \
       instead of by pointer. */ \
    builtin_define ("__GXX_MERGED_TYPEINFO_NAMES=0"); \
    builtin_define ("__GXX_TYPEINFO_EQUALITY_INLINE=0"); \
    }									\
  while (0)

/* Provide a LINK_SPEC appropriate for Antares.  Here we provide support
   for the special GCC options -static and -shared, which allow us to
   link things in one of these three modes by applying the appropriate
   combinations of options at link-time.  */

/* If ELF is the default format, we should not use /lib/elf.  */

#undef	LINK_SPEC
#define LINK_SPEC "%{!o*:-o %b} -m elf_ppc_antares -shared -no-undefined -Bsymbolic %{nostart:-e 0}"
