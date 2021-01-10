#include "ifapp.h"
#include <kimageio.h>
#include <kurl.h>
#include <klocale.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <kmessagebox.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kurl.h>
#include <kmimetype.h>
#include "imagelist.h"

static const char *description = 
	I18N_NOOP("KDE Pixie Image viewer/editor");

static KCmdLineOptions options[] =
{
    { "a", 0, 0},
    { "all", I18N_NOOP("Load all images in the current directory"), 0},
    { "l", 0, 0},
    { "load <file>", I18N_NOOP("Load all images in a given directory"), 0},
    { "+[file(s)]", I18N_NOOP("File(s) or URL(s) to open"), 0 },
    { 0, 0, 0 }
};

int main( int argc, char **argv )
{
    KAboutData aboutData( "pixie", I18N_NOOP("Pixie"), 
    "1.0", description, KAboutData::License_BSD,
    "(c) 2000 Daniel Duley <mosfet@kde.org>");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );

    KIFApplication app;
    KImageIO::registerFormats();
    KIFImageList *w = new KIFImageList;
    app.setMainWidget(w);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    for( int i = 0; i < args->count(); i++ ){
        w->slotAddURL( QFile::decodeName( args->arg(i)) );
    }
    if(args->isSet("all") || args->isSet("load")){
        QDir d(args->isSet("all") ? args->cwd() :
               args->getOption("load"));
        if(!d.exists())
            qWarning("Invalid directory specified!");
        else{
            d.setFilter(QDir::Files);
            const QFileInfoList *list = d.entryInfoList();
            QFileInfoListIterator it(*list);
            QFileInfo *fi;
            KURL url;
            while((fi=it.current())){
                url.setPath(fi->absFilePath());
                if(KMimeType::findByURL(url, true, true)->name().left(6) ==
                   "image/"){
                    w->slotAddURL(fi->absFilePath());
                }
                ++it;
            }
        }
    }
    w->slotSetTopItem();
    return(app.exec());
}
