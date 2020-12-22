#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "version.h"
#include "kview.h"


static const char *description = 
  I18N_NOOP("KDE Image viewer");

static KCmdLineOptions options[] =
{
  { "+[file(s)]", I18N_NOOP("File(s) or URL(s) to open"), 0 },
  { 0, 0, 0 }
};

int main( int argc, char *argv[] )
{
  KAboutData aboutData( "kview", I18N_NOOP("KView"), 
    KVIEW_VERSION, description, KAboutData::License_GPL, 
    "(c) 1997-2000, Mostly Sirtaj Singh Kang");
  aboutData.addAuthor( "Sirtaj Singh Kang", "Author", "taj@kde.org" );
  aboutData.addAuthor( "Simon Hausmann", 0, "shaus@neuro2.med.uni-magdeburg.de" );
  aboutData.addAuthor( "Matthias Kretz", "Maintainer", "kretz@kde.org" );
  
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KView kview;

  return kview.exec();
}
