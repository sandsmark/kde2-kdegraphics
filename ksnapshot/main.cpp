#include <kapp.h>
#include "ksnapshot.h"
#include <kimageio.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kiconloader.h>

static const char *description =
	I18N_NOOP("KDE Screenshot utility");

int main(int argc, char **argv)
{
  KAboutData aboutData( "ksnapshot", I18N_NOOP("KSnapshot"),
    KSNAPVERSION, description, KAboutData::License_GPL,
    "(c) 1997-1999, Richard J. Moore, (c) 2000, Matthias Ettrich");
  aboutData.addAuthor("Richard J. Moore",0, "rich@kde.org");
  aboutData.addAuthor("Matthias Ettrich",0, "ettrich@kde.org");
  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication app;

  KImageIO::registerFormats();

  // Create top level window
  KSnapshot *toplevel= new KSnapshot();
  app.dcopClient()->registerAs(app.name());
  app.dcopClient()->setDefaultObject( toplevel->objId() );
  toplevel->setCaption( app.makeStdCaption("") );
  toplevel->setIcon(SmallIcon("ksnapshot"));
  app.setMainWidget(toplevel);
  toplevel->show();
  return app.exec();
}

