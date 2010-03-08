/* Determine the number of screen columns needed for a character.
   Copyright (C) 2006, 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

/* Specification.  */
#include <wchar.h>

/* Get iswprint.  */
#include <wctype.h>

#include "localcharset.h"
#include "streq.h"
#include "uniwidth.h"

#undef wcwidth

int
rpl_wcwidth (wchar_t wc)
{
  /* In UTF-8 locales, use a Unicode aware width function.  */
  const char *encoding = locale_charset ();
  if (STREQ (encoding, "UTF-8", 'U', 'T', 'F', '-', '8', 0, 0, 0 ,0))
    {
      /* We assume that in a UTF-8 locale, a wide character is the same as a
	 Unicode character.  */
      return uc_width (wc, encoding);
    }
  else
    {
      /* Otherwise, fall back to the system's wcwidth function.  */
#if HAVE_WCWIDTH
      return wcwidth (wc);
#else
      return wc == 0 ? 0 : iswprint (wc) ? 1 : -1;
#endif
    }
}