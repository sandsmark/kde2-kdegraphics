/*
  ImageMagick Application Programming Interface declarations.

  Api.h depends on a number of ANSI C headers as follows:

      #include <stdio.h>
      #include <time.h>
      #include <sys/type.h>
      #include <mini-magick/api.h>

*/

#ifndef _MAGICK_API_H
#define _MAGICK_API_H

#if defined(__cplusplus) || defined(c_plusplus)
#define class  c_class
#endif

#if defined(WIN32) || defined(__CYGWIN__)
#define Export  __declspec(dllexport)
#if defined(_VISUALC_)
#pragma warning(disable : 4018)
#pragma warning(disable : 4244)
#pragma warning(disable : 4142)
#endif
#else
# define Export
#endif

#define MaxTextExtent  1664

#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#include "mini-magick/classify.h"
#include "mini-magick/image.h"
#include "mini-magick/gems.h"
#include "mini-magick/quantize.h"
#include "mini-magick/error.h"
#include "mini-magick/monitor.h"
#include "mini-magick/compress.h"
#include "mini-magick/utility.h"
#include "mini-magick/blob.h"
#include "mini-magick/cache.h"
#include "mini-magick/cache_io.h"
#include "mini-magick/memory.h"
#include "mini-magick/delegates.h"
#include "mini-magick/timer.h"
#include "mini-magick/version.h"
#else
#include "classify.h"
#include "image.h"
#include "gems.h"
#include "quantize.h"
#include "error.h"
#include "monitor.h"
#include "compress.h"
#include "utility.h"
#include "blob.h"
#include "cache.h"
#include "cache_io.h"
#include "memory.h"
#include "delegates.h"
#include "timer.h"
#include "version.h"
#endif

#endif
