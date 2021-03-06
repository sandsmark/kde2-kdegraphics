/* c-openmx.h: define OPEN_MAX, the maximum number of open files a
   process can have. Possibly dynamically determined.

Copyright (C) 1993 Karl Berry.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef C_OPENMX_H
#define C_OPENMX_H

#include <config.h>

#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#include <sys/types.h>
 
/* This is the symbol that X uses to determine if <sys/types.h> has been
 *    read, so we define it.  */
#define __TYPES__       

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#endif /* not HAVE_LIMITS_H */
 
/* Some systems may have the floating-point limits in the above.  */
#if defined (HAVE_FLOAT_H) && !defined (FLT_MAX)
#include <float.h>
#endif  

#ifndef OPEN_MAX
#ifdef _SC_OPEN_MAX
#define OPEN_MAX sysconf (_SC_OPEN_MAX)
#else /* no SC_OPEN_MAX */
#ifdef NOFILE
#define OPEN_MAX NOFILE
#else /* no NOFILE */
#define OPEN_MAX 24 /* The POSIX minimum. */
#endif /* no NOFILE */
#endif /* no _SC_OPEN_MAX */
#endif /* no OPEN_MAX */

#endif /* not C_OPENMX_H */

