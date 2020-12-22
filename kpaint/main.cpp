#include <stdio.h>
#include <unistd.h>

#include <qstring.h>
#include <qfile.h>

#include <kapp.h>
#include <kimageio.h>
#include <kdebug.h>
#include <stdlib.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kurl.h>

#include "kpaint.h"
#include "version.h"

static const char *description =
	I18N_NOOP("KDE Paint Program");

static KCmdLineOptions options[] =
{
   { "+[file]", I18N_NOOP("File or URL to open"), 0 },
   { 0, 0, 0 }
};

int main( int argc, char **argv )
{
    KAboutData aboutData(APPNAME, I18N_NOOP("KPaint"), 
    APPVERSTR, description, KAboutData::License_GPL, 
    "(c) 1996-2000, Various KDE Developers");
    aboutData.addAuthor("Richard J. Moore",0, "rich@kde.org");
    aboutData.addAuthor("Robert Williams",0, "rwilliams@jrcmaui.com");
    aboutData.addAuthor("Mario Weilguni",0, "mweilguni@kde.org");
    aboutData.addAuthor("Christian Oppel",0, "uzs84g@uni-bonn.de");
    aboutData.addAuthor("John Califf",0, "jcaliff@compuzone.net");
    aboutData.addAuthor("Laurent Montel",0, "lmontel@mandrakesoft.com");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication kpaintApp;
    KImageIO::registerFormats();
    KPaint *kp = NULL;

    if (kpaintApp.isRestored()) 
    {
        RESTORE(KPaint);
    }
    else 
    {
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        for(int i = 0; i < args->count(); i++)
        {
            kp = new KPaint(args->url(i));
            kp->show();
        }

        if (args->count() == 0)
        {
            kp = new KPaint();
            kp->show();
        }
        
        args->clear();

    }

    QObject::connect(&kpaintApp, SIGNAL(lastWindowClosed()),
		   &kpaintApp, SLOT(quit()));

    return kpaintApp.exec();
}

