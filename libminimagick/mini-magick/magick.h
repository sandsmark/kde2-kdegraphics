/*
  ImageMagick Application Programming Interface declarations.
*/
#ifndef _MAGICK_H
#define _MAGICK_H

#include <config.h>

#define _XOPEN_SOURCE  500

/*
  System include declarations.
*/
#if defined(__cplusplus) || defined(c_plusplus)
#  include <cstdio>
#  include <cstdlib>
#  include <cstdarg>
#  include <cstring>
#else
#  include <stdio.h>
#  include <stdlib.h>
/*
   This one creates compilation problems on Solaris. Yes, the bug is Solaris,
   but it still compiles and works without this #include, so I removed it.
   #  include <stdarg.h>
*/
#  include <string.h>
#endif

#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
#define fseek  fseeko
#define ftell  ftello
#endif

#include <unistd.h>

#include <ctype.h>
#include <signal.h>
#include <locale.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <setjmp.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>

#if defined(HAVE_MMAP)
#  include <sys/mman.h>
#endif
#if defined(HAVE_PTHREAD_H) && !defined(__osf__)
#  include <pthread.h>
#endif
#if defined(sysv)
#  include <sys/poll.h>
#endif
#include "mini-magick/api.h"
#include "api.h"
#include <fcntl.h>

#undef index

#endif
