/*
 * Copyright (c) 1994 Paul Vojta.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * NOTE:
 *	xdvi is based on prior work as noted in the modification history, below.
 */

/*
 * DVI previewer for X.
 *
 * Eric Cooper, CMU, September 1985.
 *
 * Code derived from dvi-imagen.c.
 *
 * Modification history:
 * 1/1986	Modified for X.10	--Bob Scheifler, MIT LCS.
 * 7/1988	Modified for X.11	--Mark Eichin, MIT
 * 12/1988	Added 'R' option, toolkit, magnifying glass
 *					--Paul Vojta, UC Berkeley.
 * 2/1989	Added tpic support	--Jeffrey Lee, U of Toronto
 * 4/1989	Modified for System V	--Donald Richardson, Clarkson Univ.
 * 3/1990	Added VMS support	--Scott Allendorf, U of Iowa
 * 7/1990	Added reflection mode	--Michael Pak, Hebrew U of Jerusalem
 * 1/1992	Added greyscale code	--Till Brychcy, Techn. Univ. Muenchen
 *					  and Lee Hetherington, MIT
 * 4/1994	Added DPS support, bounding box
 *					--Ricardo Telichevesky
 *					  and Luis Miguel Silveira, MIT RLE.
 */

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <stdlib.h>
#include <unistd.h>
#include "dviwin.h"

#include "oconfig.h"
#include "dvi.h"

#include "fontpool.h"
#include "glyph.h"



/*
 *	General utility routines.
 */

/*
 *	Print error message and quit.
 */

void oops(QString message)
{
  kdError() << i18n("Fatal Error! ") << message << endl;

  KMessageBox::error( NULL,
		      i18n("Fatal Error!\n\n") +
		      message +
		      i18n("\n\n\
This probably means that either you found a bug in KDVI,\n\
or that the DVI file, or auxiliary files (such as font files, \n\
or virtual font files) were really badly broken.\n\
KDVI will abort after this message. If you believe that you \n\
found a bug, or that KDVI should behave better in this situation\n\
please report the problem."));
  exit(1);
}

/** Either allocate storage or fail with explanation.  */

char * xmalloc(unsigned size, const char *why)
{
#ifdef DEBUG_UTIL
  kdDebug() << "Allocating " << size << " bytes for " << why << endl;
#endif

  /* Avoid malloc(0), though it's not clear if it ever actually
     happens any more.  */
  char *mem = (char *)malloc(size ? size : 1);
  
  if (mem == NULL)
    oops(QString(i18n("Cannot allocate %1 bytes for %2.")).arg(size).arg(why) );
  return mem;
}

/*
 *	Allocate bitmap for given font and character
 */

void alloc_bitmap(bitmap *bitmap)
{
  register unsigned int	size;

  /* width must be multiple of 16 bits for raster_op */
  bitmap->bytes_wide = ROUNDUP((int) bitmap->w, BITS_PER_BMUNIT) * BYTES_PER_BMUNIT;
  size = bitmap->bytes_wide * bitmap->h;
  bitmap->bits = xmalloc(size != 0 ? size : 1, "character bitmap");
}


/*
 *	Close the pixel file for the least recently used font.
 */

extern fontPool font_pool;

/*
 *	Open a file in the given mode.
 */
FILE *xdvi_xfopen(const char *filename, const char *type)
#define	TYPE	type
{
  /* Try not to let the file table fill up completely.  */
  FILE	*f = fopen(filename, "r");
  /* If the open failed, try closing a file unconditionally.
     Interactive Unix 2.2.1, at least, doesn't set errno to EMFILE
     or ENFILE even when it should.  In any case, it doesn't hurt
     much to always try.  */
  if (f == NULL) 
    f = fopen(filename, TYPE);
  return f;
}
#undef	TYPE

/*
 *
 *      Read size bytes from the FILE fp, constructing them into a
 *      signed/unsigned integer.
 *
 */

unsigned long num(FILE *fp, int size)
{
	register long x = 0;

	while (size--) x = (x << 8) | one(fp);
	return x;
}

long snum(FILE *fp, int size)
{
	register long x;

#ifdef	__STDC__
	x = (signed char) getc(fp);
#else
	x = (unsigned char) getc(fp);
	if (x & 0x80) x -= 0x100;
#endif
	while (--size) x = (x << 8) | one(fp);
	return x;
}
