#include <qdir.h>

#include <kaboutdata.h>
#include <kapp.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kurl.h>

#include "kgvshell.h"
#include "version.h"

static KCmdLineOptions options[] =
{
    { "+file(s)",   I18N_NOOP(" Files to load" ),   0 },
    { 0,	    0,				    0 }
};

static const char* description = I18N_NOOP( "Viewer for Postscript (.ps, .eps) and Portable Document Format (.pdf) files.");


int main( int argc, char** argv )
{
    KAboutData about( "kghostview", I18N_NOOP( "KGhostView"), 
		      KGHOSTVIEW_VERSION, description, KAboutData::License_GPL,
		      "(C) 1998 Mark Donohoe, (C) 1999-2000 David Sweet, "
		      "(C) 2000-2001 Wilco Greven",
		      I18N_NOOP( "KGhostView displays, prints, and saves "
				 "Postscript and PDF files.\n"
				 "Based on original work by Tim Theisen."));
    about.addAuthor( "Wilco Greven",
		     I18N_NOOP( "Current maintainer" ),
		     "greven@kde.org" );
    about.addAuthor( "David Sweet", 
		     I18N_NOOP( "Maintainer 1999-2000" ),
		     "dsweet@kde.org",
		     "http://www.andamooka.org/~dsweet" );
    about.addAuthor( "Mark Donohoe",
		     I18N_NOOP( "Original author" ) );
    about.addAuthor( "David Faure",
		     I18N_NOOP( "Basis for shell") );
    about.addAuthor( "Daniel Duley",
		     I18N_NOOP( "Port to KParts" ) );
    about.addAuthor( "Espen Sand",
		     I18N_NOOP( "Dialog boxes" ) );

    KCmdLineArgs::init( argc, argv, &about );
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

    if( kapp->isRestored() )
	RESTORE( KGVShell )
    else {
	KGVShell* shell = new KGVShell;
	if( args->count() == 1 ) {
	    if( QString( args->arg(0) ) == "-" )  
		shell->openStdin();
	    else {
		KURL url( QDir::currentDirPath() + "/", args->arg(0) );
		shell->openURL( url );
	    }
	}
	shell->show();
    }
    return app.exec();
}
