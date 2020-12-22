#include <kcmdlineargs.h>
#include <kapp.h>
#include <kurl.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <qdir.h>

#include <stdlib.h>

#include "kviewshell.h"


static KCmdLineOptions options[] =
{
  { "+part",    I18N_NOOP("The part to use"), 0},
  { "+file(s)", I18N_NOOP("Files to load"), 0 },
  { 0,0,0 }
};


static const char *description = I18N_NOOP("Generic framework for viewer applications.");


int main(int argc, char **argv)
{
  KAboutData about ("kviewshell", I18N_NOOP("KViewShell"), "0.1",
		    description, KAboutData::License_GPL,
		    "(C) 2000, Matthias Hoelzer-Kluepfel",
		    I18N_NOOP("Displays various document formats. "
		    "Based on original code from KGhostView."));
  about.addAuthor ("Matthias Hoelzer-Kluepfel", I18N_NOOP("Framework"),
		   "mhk@caldera.de");
  about.addAuthor ("David Sweet", 
		   I18N_NOOP("KGhostView Maintainer"),
		   "dsweet@kde.org",
		   "http://www.chaos.umd.edu/~dsweet");
  about.addAuthor ("Mark Donohoe",
		   I18N_NOOP("KGhostView Author"));
  about.addAuthor ("Markku Hihnala",
		   I18N_NOOP("Navigation widgets"));
  about.addAuthor ("David Faure",
		   I18N_NOOP("Basis for shell"));
  about.addAuthor ("Daniel Duley",
		   I18N_NOOP("Port to KParts"));
  about.addAuthor ("Espen Sand",
		   I18N_NOOP("Dialogs boxes"));

  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions( options ); // Add my own options.
  KApplication app;
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();


  if (args->count() < 1)
    {
      args->usage();
      exit(-1);
    }

  KViewShell *shell = new KViewShell(args->arg(0), &about);
  

  if ( args->count() > 1 )
    shell->openURL( args->url(1) );

  shell->show();
  return app.exec();
}
