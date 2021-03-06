/***************************************************************************
	     kscanoptset.cpp -  store a set of scan options
                             -------------------
    begin                : Wed Oct 13 2000
    copyright            : (C) 2000 by Klaas Freitag
    email                : Klaas.Freitag@SuSE.de

    $Id: kscanoptset.cpp 99317 2001-05-29 15:49:14Z freitag $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <qstring.h>
#include <qasciidict.h>
#include <qdict.h>
#include <kdebug.h>
#include <kconfig.h>

#include "kscandevice.h"
#include "kscanoption.h"
#include "kscanoptset.h"


KScanOptSet::KScanOptSet( const QCString& setName )
{
  name = setName;

  setAutoDelete( false );

  strayCatsList.setAutoDelete( true );
}



KScanOptSet::~KScanOptSet()
{
   /* removes all deep copies from backupOption */
   strayCatsList.clear();
}



KScanOption *KScanOptSet::get( const char *name ) const
{
  KScanOption *ret = 0;

  ret = (*this) [name];

  return( ret );
}

bool KScanOptSet::backupOption( const KScanOption& opt )
{
  bool retval = true;

  /** Allocate a new option and store it **/
  const QCString& optName = opt.getName();
  if( !optName )
    retval = false;

  if( retval )
  {
     KScanOption *newopt = find( optName );

     if( newopt )
     {
	/** The option already exists **/
	/* Copy the new one into the old one. TODO: checken Zuweisungoperatoren OK ? */
	*newopt = opt;
     }
     else
     {
	const QCString& qq = opt.get();
	kdDebug(29000) << "Value is now: <" << qq << ">" << endl;
	const KScanOption *newopt = new KScanOption( opt );

	strayCatsList.append( newopt );
	
	if( newopt )
	{
	   insert( optName, newopt );
	} else {
	   retval = false;
	}
     }
  }

  return( retval );

}

/* */
void KScanOptSet::saveConfig( const QString& scannerName, const QString& configName,
			      const QString& descr )
{
   KConfig scanConfig( QString( "ScanSettings-" ) + scannerName );
   QString cfgName = configName;
   
   if( configName.isNull() || configName.isEmpty() )
      cfgName = "default";
   
   scanConfig.setGroup( cfgName );

   scanConfig.writeEntry( "description", descr );
   QAsciiDictIterator<KScanOption> it( *this);
 
    while ( it.current() ) {
       const QString line = it.current() -> configLine();
       kdDebug(29000) << "writing <" << line << ">" << endl;

       scanConfig.writeEntry( QString(it.current()->getName()), line );
       
       ++it;
    }
    
}


/* END */
