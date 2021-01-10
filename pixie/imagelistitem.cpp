#include "imagelistitem.h"

#include <qfile.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qpixmap.h>

#include <kio/netaccess.h>
#include <kdebug.h>

KIFImageListItem::KIFImageListItem(QListBox *listbox, const KURL &url)
    : QListBoxText(listbox, url.url())
{
    u = url;
}

KIFImageListItem::~KIFImageListItem()
{
    clearTempData();
}

QString KIFImageListItem::fileName()
{

    if(u.isLocalFile()){
        return(u.path());
    }
    if(tempFileStr.isEmpty())
        KIO::NetAccess::download(u.url(), tempFileStr);
    return(tempFileStr);
}

void KIFImageListItem::clearTempData()
{
    if(!tempFileStr.isNull())
    {
        kdDebug() << "Cleaning temp file for " << u.url() << ", filename "
                    << tempFileStr << endl;
        KIO::NetAccess::removeTempFile(tempFileStr);
    }
}


