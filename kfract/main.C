
    /*

    main.C  version 0.1.2

    Copyright (C) 1997 Uwe Thiem   
                       uwe@uwix.alt.na

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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */


#include "kfract.h"
#include "version.h"
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kimageio.h>


static const char *description = 
	I18N_NOOP("Little toy to generate fractal images.");

int main( int argc, char **argv )
{
  KAboutData aboutData( "kfract", I18N_NOOP("KDE Fractal Generator"), 
    KFRACT_VERSION, description, KAboutData::License_GPL, 
    "(c) 1999-2000, Uwe Thiem");
  aboutData.addAuthor("Uwe Thiem",0, "uwe@kde.org");
  KCmdLineArgs::init( argc, argv, &aboutData );

  KFract kfract;

  KImageIO::registerFormats();

  return kfract.exec();
}

