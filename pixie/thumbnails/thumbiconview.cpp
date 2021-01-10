#include "thumbiconview.h"
#include <qfile.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qdragobject.h>
#include <qpopupmenu.h>
#include <kapp.h>
#include <kmimetype.h>
#include <kurl.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <qintdict.h>
#include <kstddirs.h>

KIFThumbView::KIFThumbView(QWidget *parent, const char *name)
    : QIconView(parent, name)
{

    setGridX(95);
    setGridY(95);
    setAutoArrange(false);
    setSorting(false);
    setResizeMode(Adjust);
    setWordWrapIconText(true);
    setSelectionMode(QIconView::Extended);
    connect(this, SIGNAL(dropped(QDropEvent *, const QValueList<QIconDragItem>&)),
            this, SLOT(slotIconDrop(QDropEvent *, const QValueList<QIconDragItem>&)));
    iconOnly = false;
}

KIFThumbView::~KIFThumbView()
{
    qWarning("In thumb view destructor");
    stopProcessing = true;
}

void KIFThumbView::drawBackground(QPainter *p, const QRect &r)
{
    QIconView::drawBackground(p, r);
}

void KIFThumbView::setIconOnly(bool en)
{
    iconOnly = en;
}


void KIFThumbView::slotStopClicked()
{
    stopProcessing = true;
}

void KIFThumbView::update(const QString &dirStr, IconSize iSize,
                          int sortType, bool createThumbs,
                          bool dupFirst)
{
    int iSort;
    stopProcessing = false;
    emit enableStopButton(true);
    currentDir = dirStr;
    clear();

    switch(sortType){
    case 1:
        iSort = QDir::Name | QDir::Reversed;
        break;
    case 2:
        iSort = QDir::Time;
        break;
    case 3:
        iSort = QDir::Time | QDir::Reversed;
        break;
    case 4:
    case 6:
        iSort = QDir::Size;
        break;
    case 5:
        iSort = QDir::Size | QDir::Reversed;
        break;
    default:
        iSort = QDir::Name;
        break;
    }

    QDir dir(dirStr);
    if(!dir.exists()){
        // TODO
        qWarning("Opps: Invalid dir given to KIFThumbView::update");
        dir = QDir::home();
    }
    dir.setSorting(iSort | QDir::DirsFirst);
    dir.setFilter(QDir::Files | QDir::Dirs);

    QString thumbStr;
    int iconSize;
    switch(iSize){
    case Large:
        iconSize = 90;
        thumbStr = "large/";
        break;
    case Medium:
        iconSize = 64;
        thumbStr = "med/";
        break;
    case Small:
    default:
        iconSize = 48;
        thumbStr = "small/";
        break;
    }

    bool persistantThumbs = createThumbs;
    if(createThumbs){
        if(!QFile::exists(dir.absPath() + "/.pics"))
            if(!dir.mkdir(dir.absPath() + "/.pics"))
                persistantThumbs = false;
        if(!QFile::exists(dir.absPath() + "/.pics/" + thumbStr))
            if(!dir.mkdir(dir.absPath() + "/.pics/" + thumbStr))
                persistantThumbs = false;
    }
    thumbStr = dir.absPath() + "/.pics/" + thumbStr;

    const QFileInfoList *fileList;
    if(dupFirst)
        fileList = dupSizeFirst(dir.entryInfoList());
    else
        fileList = dir.entryInfoList();
    
    QFileInfoListIterator it(*fileList);
    QFileInfo *fi;
    int percent = 0;
    int current = 1;
    int count = it.count();
    QPixmap currentPix;
    bool isImage;
    KURL url;
    while((fi = it.current()) && !stopProcessing){
        url.setPath("file:" + fi->absFilePath());
        isImage = KMimeType::findByURL(url, true, true)->name().left(6) ==
            "image/";
        if(isImage){
            if(QFile::exists(thumbStr + fi->fileName())&&
              QFileInfo(thumbStr+fi->fileName()).lastModified() > fi->lastModified()){
                //qWarning("update : found existing thumbnail");
                currentPix.load(thumbStr + fi->fileName());
            }
            else{
                if(createThumbs){
                    currentPix.load(fi->absFilePath());
                    if(currentPix.width() > iconSize ||
                       currentPix.height() > iconSize){
                        QImage image = currentPix.convertToImage();
                        if(image.width() > image.height()){
                            float percent = (((float)iconSize)/image.width());
                            int h = (int)(image.height()*percent);
                            image = image.smoothScale(iconSize, h);
                        }
                        else{
                            float percent = (((float)iconSize)/image.height());
                            int w = (int)(image.width()*percent);
                            image = image.smoothScale(w, iconSize);
                        }
                        currentPix.convertFromImage(image);
                    }
                }
                if(persistantThumbs)
                    currentPix.save(thumbStr + fi->fileName(), "PNG");
            }
        }
        if(currentPix.isNull() && (fi->isDir() || (isImage || !iconOnly))){
            if(iSize == Small)
                currentPix = KMimeType::pixmapForURL(KURL("file:"+fi->absFilePath()),
                                                     0, KIcon::Desktop, KIcon::SizeMedium);
            else
                currentPix = KMimeType::pixmapForURL(KURL("file:"+fi->absFilePath()),
                                                     0, KIcon::Desktop, KIcon::SizeLarge);
        }
        KIFThumbViewItem *i;
        if(fi->fileName() == ".")
            i = NULL;
        else if(fi->isDir()){
            if(fi->fileName() == "..")
                i = new KIFThumbViewItem(this, i18n("Go Up"), currentPix, true);
            else
                i = new KIFThumbViewItem(this, fi->fileName(), currentPix);
            i->setDropEnabled(true);
        }
        else if(isImage || !iconOnly){
            i = new KIFThumbViewItem(this, fi->fileName(), currentPix);
            i->setDropEnabled(false);
        }
        else{
            i = NULL;
        }
        //qWarning("Inserted %s", fi->fileName().latin1());
        //qWarning("item %d of %d : percent %d", current, count,
        //        (int)(((float)current/count)*100));
        if((int)(((float)current/count)*100) !=  percent){
            percent =  (int)(((float)current/count)*100);
            emit updateProgress(percent);
            slotUpdate();
            if(i)
                ensureItemVisible(i);
        }
        kapp->processEvents();
        ++it;
        ++current;
        currentPix.resize(0, 0);
    }
    emit enableStopButton(false);
    if(dupFirst)
        delete fileList;
}

// info list from QDir is const, so we need to make a copy in dest
QFileInfoList* KIFThumbView::dupSizeFirst(const QFileInfoList *src)
{
    QFileInfoList *dest = new QFileInfoList;
    dest->setAutoDelete(true);
    QFileInfoListIterator it(*src);
    QFileInfo *fi;

    QValueList<int> sizeList; // list of sizes we encountered
    while((fi = it.current())){
        if(sizeList.find(fi->size()) == sizeList.end()){
            dest->append(new QFileInfo(*fi));
            sizeList.append(fi->size());
        }
        else{
            // duplicate, previous item should be the same size unless there's
            // more than one match.
            if(dest->count() && dest->at(dest->count()-1) &&
               dest->at(dest->count()-1)->size() == fi->size()){
                QFileInfo *tmp = dest->take(dest->count()-1);
                dest->prepend(tmp); // add this to top
            }
            dest->prepend(new QFileInfo(*fi));
        }
        ++it;
    }
    return(dest);
}

QDragObject* KIFThumbView::dragObject()
{
    QStrList uriList;
    KIFThumbViewItem *i = (KIFThumbViewItem *)firstItem();
    KIFThumbViewItem *firstSelItem = NULL; // for pixmap
    while(i){
        if(i->isSelected() && !i->parentDir()){
            uriList.append(QUriDrag::localFileToUri(currentDir + "/" +
                                                    i->text()));
            if(!firstSelItem)
                firstSelItem = i;
        }
        i = (KIFThumbViewItem *)i->nextItem();
    }

    QUriDrag *uri = new QUriDrag(uriList, this);
    if(uriList.count() == 1)
        uri->setPixmap(*firstSelItem->pixmap());
    else{
        KIconLoader *ldr = KGlobal::iconLoader();
        uri->setPixmap(ldr->loadIcon("kmultiple", KIcon::NoGroup, KIcon::SizeMedium));
    }
    return(uri);
}

void KIFThumbView::slotIconDrop(QDropEvent *ev,
                                const QValueList<QIconDragItem> &)
{
    QStringList fileList;
    if(!QUriDrag::decodeToUnicodeUris(ev, fileList)){
        qWarning("Pixie: Can't decode drop.");
        return;
    }

    if(ev->source() == this){
        qWarning("Pixie: Dropped thumbnails onto their own directory.");
        return;
    }
    if(!fileList.count())
        return;
    
    int op;
    QPopupMenu opPopup;
    opPopup.insertItem(i18n("&Copy Here"), 1);
    opPopup.insertItem(i18n("&Move Here"), 2);
    opPopup.insertItem(i18n("&Link Here"), 3);
    QPoint pos = contentsToViewport(ev->pos());
    op = opPopup.exec(viewport()->mapToGlobal(pos));
    switch(op){
    case 1:
        ev->setAction(QDropEvent::Copy);
        break;
    case 2:
        ev->setAction(QDropEvent::Move);
        break;
    case 3:
        ev->setAction(QDropEvent::Link);
        break;
    default:
        return;
    }

    KIFThumbViewItem *item = (KIFThumbViewItem *)findItem(ev->pos());
    QString destStr("file:" + currentDir);
    if(item && item->dropEnabled()){
        if(item->parentDir())
            destStr += "/../";
        else
            destStr += "/" + item->text() + "/";
    }
    KURL dest(destStr);
    KIO::Job *job = 0L;
    switch(ev->action()){
    case QDropEvent::Move:
        job = KIO::move(fileList, dest, false);
        connect(job, SIGNAL(result(KIO::Job *)), this,
                SLOT(slotDropJobReload(KIO::Job *)));
        break;
    case QDropEvent::Copy:
        job = KIO::copy(fileList, dest, false);
        connect(job, SIGNAL(result(KIO::Job *)), this,
                SLOT(slotDropJobFinished(KIO::Job *)));
        break;
    case QDropEvent::Link:
        KIO::link(fileList, dest);
        emit updateMe();
        break;
    default:
        break;
    }

}

void KIFThumbView::slotDropJobFinished(KIO::Job *job)
{
    if(job->error())
        job->showErrorDialog(this);
}

void KIFThumbView::slotDropJobReload(KIO::Job *job)
{
    if(job->error())
        job->showErrorDialog(this);
    emit updateMe();
}

bool KIFThumbView::isImageExtension(const QString &extStr)
{
    QString str = extStr.lower();
    if(str == "png" || str == "xpm" || str == "tiff" || str == "tif" ||
       str == "jpg" || str == "jpeg" || str == "gif")
        return(true);
    return(false);
}

void KIFThumbViewItem::dropped(QDropEvent *ev,
                               const QValueList<QIconDragItem> &)
{
    if(!dropEnabled())
        return;

    QStringList fileList;
    if(!QUriDrag::decodeToUnicodeUris(ev, fileList)){
        qWarning("Pixie: Can't decode drop.");
        return;
    }

    if(!fileList.count())
        return;
    
    int op;
    QPopupMenu opPopup;
    opPopup.insertItem(i18n("&Copy Here"), 1);
    opPopup.insertItem(i18n("&Move Here"), 2);
    opPopup.insertItem(i18n("&Link Here"), 3);
    QPoint pos = iconView()->contentsToViewport(ev->pos());
    op = opPopup.exec(iconView()->viewport()->mapToGlobal(pos));
    switch(op){
    case 1:
        ev->setAction(QDropEvent::Copy);
        break;
    case 2:
        ev->setAction(QDropEvent::Move);
        break;
    case 3:
        ev->setAction(QDropEvent::Link);
        break;
    default:
        return;
    }

    QString destStr("file:" + ((KIFThumbView *)iconView())->currentPath());
    if(parentDir())
        destStr += "/../";
    else
        destStr += "/" + text() + "/";

    KURL dest(destStr);
    // no need to pay attention to job finishing
    switch(ev->action()){
    case QDropEvent::Move:
        (void)KIO::move(fileList, dest, false);
        break;
    case QDropEvent::Copy:
        (void)KIO::copy(fileList, dest, false);
        break;
    case QDropEvent::Link:
        (void)KIO::link(fileList, dest);
        break;
    default:
        break;
    }
}

bool KIFThumbViewItem::acceptDrop(const QMimeSource *e) const
{
    return(dropEnabled() && QUriDrag::canDecode(e));
}

    

    


#include "thumbiconview.moc"


