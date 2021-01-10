#include "imagelist.h"
#include "imagelistitem.h"
//#include "imagecolor.h"
//#include "channeledit.h"
//#include "singlecolors.h"
#include "htmlexport.h"
#include "pluginmenu.h"
#include "moviewindow.h"
#include "sizedialog.h"
#include "slidedialog.h"
#include "dndlistbox.h"
#include "screengrab.h"
#include "miscbindings.h"
#include <qclipboard.h>
#include <qprinter.h>
#include <qpainter.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kstdaccel.h>
#include <ifapp.h>
#include <kimageeffect.h>
#include <kimageio.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kaccel.h>
#include <kaccelmenu.h>
#include <kaction.h>

#include <kdebug.h>

#include <ltdl.h>

// plugin hacks
KIFImageList *imageList;

KIFImageList::KIFImageList(const char *name)
    : KTMainWindow(name)
{
    setAutoSaveSettings("PixieList");
    fileList = new KIFDndListBox(this);
    connect(fileList, SIGNAL(urlDropped(const QString &)), this,
            SLOT(slotAddURL(const QString &)));
    connect(fileList, SIGNAL(dropFinished()), this,
            SLOT(slotDropFinished()));
    
    setCentralWidget(fileList);
    setCaption(i18n("KDE Pixie Viewer/Editor"));

    imageWindow = new KIFImageWindow;

    // custom keys - messy, but menu accels seem only to work with modifiers
    keys = new KAccel(this);
    keys->insertItem(i18n("Double Size"), "Double", Key_Greater);
    keys->insertItem(i18n("Half Size"), "Half", Key_Less);
    keys->insertItem(i18n("Brighten"), "Brighten", Key_Plus);
    keys->insertItem(i18n("Dim"), "Dim", Key_Minus);
    keys->readSettings();

    KMenuBar *mbar = menuBar();
    fileMnu = new QPopupMenu(this);
    fileMnu->insertItem(i18n("Take a screenshot..."), this, SLOT(slotGrab()));
    //fileMnu->insertItem(i18n("Make HTML thumbnails..."), this,
    //                    SLOT(slotGenerateHtml()));
    slideShowID = fileMnu->insertItem(i18n("Slide Show..."), this,
                                      SLOT(slotSlideShow()));

    fileMnu->insertItem(KStdAccel::description(KStdAccel::Open), this,
                        SLOT(slotOpen()), KStdAccel::key(KStdAccel::Open));
    fileMnu->insertItem(KStdAccel::description(KStdAccel::Save), this,
                        SLOT(slotSave()), KStdAccel::key(KStdAccel::Save));
    fileMnu->insertItem(i18n("Save &As..."), this, SLOT(slotSaveAs()));
    fileMnu->insertItem(KStdAccel::description(KStdAccel::Print), this,
                        SLOT(slotPrint()), KStdAccel::key(KStdAccel::Print));
    fileMnu->insertSeparator();
    fileMnu->insertItem(i18n("E&xit"), this, SLOT(slotQuit()));
    mbar->insertItem(i18n("&File"), fileMnu);

    int id;
    QPopupMenu *editMnu = new QPopupMenu(this);
    editMnu->insertItem(KStdAccel::description(KStdAccel::Copy), this, SLOT(slotCopy()),
                        KStdAccel::key(KStdAccel::Copy));
    editMnu->insertItem(KStdAccel::description(KStdAccel::Paste), this, SLOT(slotPaste()),
                        KStdAccel::key(KStdAccel::Paste));
    editMnu->insertItem(KStdAccel::description(KStdAccel::SelectAll), this,
                        SLOT(slotSelectAll()),
                        KStdAccel::key(KStdAccel::SelectAll));
    editMnu->insertItem(i18n("Unselect All"), this, SLOT(slotDeselectAll()));
    editMnu->insertSeparator();
    editMnu->insertItem(i18n("Crop"), this, SLOT(slotCrop()));
    editMnu->insertItem(i18n("Scale..."), this, SLOT(slotScale()));
    editMnu->insertItem(i18n("10% Larger"), this, SLOT(slotScaleIn()));
    editMnu->insertItem(i18n("10% Smaller"), this, SLOT(slotScaleOut()));
    id = editMnu->insertItem(i18n("Double Size"), this, SLOT(slotDouble()));
    keys->changeMenuAccel(editMnu, id, "Double");
    id = editMnu->insertItem(i18n("Half Size"), this, SLOT(slotShrink()));
    keys->changeMenuAccel(editMnu, id, "Half");
    editMnu->insertItem(i18n("Horizontal Flip"), this, SLOT(slotMirrorHoriz()));
    editMnu->insertItem(i18n("Vertical Flip"), this, SLOT(slotMirrorVert()));
    editMnu->insertItem(i18n("Rotate 45 degrees"), this, SLOT(slotRotate45()));
    editMnu->insertItem(i18n("Invert"), this, SLOT(slotInvert()));
    keys->readSettings();
    mbar->insertItem(i18n("&Edit"), editMnu);

    QPopupMenu *optionMnu = new QPopupMenu(this);
    mbar->insertItem(i18n("&Configure"), optionMnu);

    QPopupMenu *windowMnu = new QPopupMenu(this);
    windowMnu->insertItem(i18n("Thumbnail viewer"), this,
                          SLOT(slotThumbnails()));
    //windowMnu->insertItem(i18n("HTML thumbnail export"), this,
    //                      SLOT(slotGenerateHtml()));
    windowMnu->insertItem(i18n("Image color editor"), this,
                          SLOT(slotImageColor()));
    windowMnu->insertItem(i18n("Individual color editor"), this,
                          SLOT(slotSingleColor()));

    mbar->insertItem(i18n("&Windows"), windowMnu);

    QPopupMenu *effectMnu = new KIFPluginMenu(imageWindow, this);
    mbar->insertItem(i18n("Effec&ts"), effectMnu);

    mbar->insertSeparator();
    mbar->insertItem(i18n("&Help"),
                     helpMenu(i18n("KDE Pixie Viewer/Editor\n(C)Daniel M. Duley <mosfet@kde.org>"), false));

    KToolBar *tBar = new KToolBar(this);
    tBar->insertButton(BarIcon("fileopen"), 0, SIGNAL(clicked()), this,
                       SLOT(slotOpen()));
    tBar->insertButton(BarIcon("filesave"), 0, SIGNAL(clicked()), this,
                       SLOT(slotSave()));
    tBar->insertButton(BarIcon("fileprint"), 0, SIGNAL(clicked()), this,
                       SLOT(slotPrint()));
    tBar->insertLineSeparator();
    tBar->insertButton(BarIcon("back"), 0, SIGNAL(clicked()), this,
                       SLOT(slotBack()));
    tBar->insertButton(BarIcon("forward"), 0, SIGNAL(clicked()), this,
                       SLOT(slotForward()));
    tBar->insertButton(BarIcon("reload"), 0, SIGNAL(clicked()), this,
                       SLOT(slotReload()));
    tBar->insertLineSeparator();
    tBar->insertButton(BarIcon("editcopy"), 0, SIGNAL(clicked()), this,
                       SLOT(slotCopy()));
    tBar->insertButton(BarIcon("editpaste"), 0, SIGNAL(clicked()), this,
                       SLOT(slotPaste()));
    addToolBar(tBar);

    tBar = new KToolBar(this);
    tBar->insertButton(UserIcon("brightness+"), 0, SIGNAL(clicked()), this,
                       SLOT(slotBright()));
    tBar->insertButton(UserIcon("brightness-"), 0, SIGNAL(clicked()), this,
                       SLOT(slotDim()));
    tBar->insertButton(UserIcon("contrast+"), 0, SIGNAL(clicked()), this,
                       SLOT(slotIncContrast()));
    tBar->insertButton(UserIcon("contrast-"), 0, SIGNAL(clicked()), this,
                       SLOT(slotDecContrast()));
    tBar->insertLineSeparator();
    tBar->insertButton(UserIcon("scaleup"), 0, SIGNAL(clicked()), this,
                       SLOT(slotScaleIn()));
    tBar->insertButton(UserIcon("scaledown"), 0, SIGNAL(clicked()), this,
                       SLOT(slotScaleOut()));
    tBar->insertButton(UserIcon("fliph"), 0, SIGNAL(clicked()), this,
                       SLOT(slotMirrorHoriz()));
    tBar->insertButton(UserIcon("flipv"), 0, SIGNAL(clicked()), this,
                       SLOT(slotMirrorVert()));

    tBar->insertLineSeparator();
    tBar->insertButton(UserIcon("play"), 0, SIGNAL(clicked()), this,
                       SLOT(slotPlayAni()));
    addToolBar(tBar);

    imageWindow->show();

    connect(imageWindow, SIGNAL(toggleFileList()), this,
            SLOT(slotToggleFileList()));
    connect(fileList, SIGNAL(selected(QListBoxItem *)), this,
            SLOT(slotURLSelected(QListBoxItem *)));
    resize(390, 250);

    connect(kifapp(), SIGNAL(hideAllWindows()), this, SLOT(slotHide()));
    connect(kifapp(), SIGNAL(showAllWindows()), this, SLOT(slotShow()));

    imageList = this;
    dlregistered = false;
    singleColorDlg = NULL;
    showTimer = NULL;
    imageColorDlg = NULL;
}

KIFImageList::~KIFImageList()
{
    keys->writeSettings();
    kdDebug() << "KIFImageList::~KIFImageList()" << endl;
}

void KIFImageList::slotToggleFileList()
{
    kdDebug() << "In slotToggleFileList" << endl;
    if(isVisible())
        hide();
    else
        show();
}

void KIFImageList::slotThumbnails()
{
    static lt_dlhandle handle = 0;
    static void *(*init_ptr)(void (*)(const QString &)) = NULL;

    if(!init_ptr){
        if(!dlregistered){
            dlregistered = true;
            lt_dlinit();
        }
        QString libPath = KGlobal::dirs()->findResource("module",
                                                        "libpixie_thumb.la");
        handle = lt_dlopen(libPath.latin1());
        if(!handle){
            KMessageBox::error(this,
                               i18n("Cannot dynamic load the thumbnail viewer!"),
                               i18n("Pixie Plugin Error"));
            return;
        }
        lt_ptr init_func = lt_dlsym(handle, "init");
        init_ptr = (void * (*)(void (*)(const QString &)))init_func;
        if(!init_ptr){
            KMessageBox::error(this,
                               i18n("Invalid thumnail plugin!"),
                               i18n("Pixie Plugin Error"));
            lt_dlclose(handle);
        }
        else
            init_ptr(pluginSetURL);
    }
    else{
        kdDebug() << "Pixie: thumbnail plugin already loaded." << endl;
        init_ptr(pluginSetURL);
    }

}

void KIFImageList::loadColorPlugin()
{
    static lt_dlhandle handle = 0;
    static void *(*init_ptr)(QImage* (*)(), void (*)()) = NULL;

    if(!init_ptr){
        kdDebug() << "Pixie: Loading color plugin" << endl;
        if(!dlregistered){
            dlregistered = true;
            lt_dlinit();
        }
        QString libPath = KGlobal::dirs()->findResource("module",
                                                        "libpixie_color.la");
        handle = lt_dlopen(libPath.latin1());
        if(!handle){
            KMessageBox::error(this,
                               i18n("Cannot dynamic load the color editor!"),
                               i18n("Pixie Plugin Error"));
            return;
        }
        lt_ptr init_func = lt_dlsym(handle, "init");
        init_ptr = (void * (*)(QImage* (*)(), void (*)()))init_func;
        if(!init_ptr){
            KMessageBox::error(this,
                               i18n("Invalid color plugin!"),
                               i18n("Pixie Plugin Error"));
            lt_dlclose(handle);
        }
        else{
            init_ptr(currentImage, updateImage);
            lt_ptr single_func = lt_dlsym(handle, "runSingleColor");
            singleColorDlg = (void *(*)())single_func;
            lt_ptr image_func = lt_dlsym(handle, "runImageColor");
            imageColorDlg = (void *(*)())image_func;
            if(!singleColorDlg || !imageColorDlg){
                KMessageBox::error(this,
                                   i18n("Invalid color plugin!"),
                                   i18n("Pixie Plugin Error"));
                lt_dlclose(handle);
                init_ptr = NULL;
                singleColorDlg = NULL;
                imageColorDlg = NULL;
            }
        }

    }
    else{
        kdDebug() << "Pixie: Color plugin already loaded." << endl;
        //init_ptr(currentImage, updateImage);
        return;
    }
    return;
}

void KIFImageList::slotAddURL(const QString &urlStr)
{
    int idx;
    if(QFile::exists(urlStr)){
        QFileInfo fi(urlStr);
        QString tmpStr("file:");
        tmpStr += fi.absFilePath();
        idx = findURLIndex(tmpStr);
        if(idx == -1)
            (void) new KIFImageListItem(fileList, KURL(tmpStr));
    }
    else{
        idx = findURLIndex(urlStr);
        if(idx == -1)
            (void) new KIFImageListItem(fileList, KURL(urlStr));
    }
}

void KIFImageList::slotURLSelected(QListBoxItem *i)
{
    kdDebug() << "URL selected: " << ((KIFImageListItem *)i)->fileName() << endl;
    imageWindow->slotSetFile(((KIFImageListItem *)i)->fileName());
}

void KIFImageList::slotAddAndSetURL(const QString &urlStr)
{
    int idx;
    KIFImageListItem *i;
    if(QFile::exists(urlStr)){
        QFileInfo fi(urlStr);
        QString tmpStr("file:");
        tmpStr += fi.absFilePath();
        idx = findURLIndex(tmpStr);
        if(idx == -1)
            i = new KIFImageListItem(fileList, KURL(tmpStr));
        else
            i = (KIFImageListItem *)fileList->item(idx);
    }
    else{
        idx = findURLIndex(urlStr);
        if(idx == -1)
            i =  new KIFImageListItem(fileList, KURL(urlStr));
        else
            i = (KIFImageListItem *)fileList->item(idx);
    }
    fileList->setCurrentItem(i);
    imageWindow->slotSetFile(i->fileName());
}

void KIFImageList::slotSetTopItem()
{
    if(fileList->count() > 0){
        fileList->setCurrentItem(0);
        imageWindow->slotSetFile(((KIFImageListItem *)fileList->item(0))->
                                 fileName());
    }
}

void KIFImageList::slotOpen()
{
    KURL u =
        KFileDialog::getOpenURL(QString::null,
                                     KImageIO::pattern(KImageIO::Reading),
                                     this);
    QString fileStr = u.path();
    if(!fileStr.isNull())
        slotAddAndSetURL(fileStr);
}

void KIFImageList::slotSave()
{
    int currentItem = fileList->currentItem();
    if(currentItem != -1){
        QString fileStr=
            ((KIFImageListItem *)fileList->item(currentItem))->fileName();
        imageWindow->image()->save(fileStr,
                                   KImageIO::type(fileStr).latin1());
    }
    else
        KMessageBox::error(this, i18n("No file selected!"));
}

void KIFImageList::slotSaveAs()
{
    KURL u  =
        KFileDialog::getSaveURL(fileList->currentText(),
                                     KImageIO::pattern(KImageIO::Writing),
                                     this);

    QString fileStr = u.path();
    if(!fileStr.isNull()){
        imageWindow->image()->save(fileStr, KImageIO::type(fileStr).latin1());
    }
}

void KIFImageList::slotPrint()
{
    QPrinter printer;
    if(printer.setup(this)){
        QPainter p(&printer);
        p.drawPixmap(0, 0, *imageWindow->pixmap());
    }
}

void KIFImageList::slotCopy()
{
    QClipboard *cb = QApplication::clipboard();
    QImage *img = imageWindow->image();
    if(!img->isNull()){
        kdDebug() << "Copying image" << endl;
        cb->setImage(*img);
    }
    else{
        kdDebug() << "Copying pixmap" << endl;
        cb->setPixmap(*imageWindow->pixmap());
    }

}

void KIFImageList::slotPaste()
{
    kdDebug() << "In slotPaste" << endl;
    QImage newImage = QApplication::clipboard()->image();
    if(!newImage.isNull()){
        *imageWindow->image() = newImage;
        imageWindow->slotUpdateFromImage();
    }
    else
        kdWarning() << "Unable to process clipboard data" << endl;
}

void KIFImageList::slotScale()
{
    QImage *image = imageWindow->image();
    KIFSizeDialog dlg(image->width(), image->height(), this);
    if(dlg.exec() == QDialog::Accepted){
        kdDebug() << "Scaling to " << dlg.resultWidth()
                    << ", " << dlg.resultHeight() << endl;
        *image = image->smoothScale(dlg.resultWidth(),
                                    dlg.resultHeight());
        imageWindow->slotUpdateFromImage();

    }
}

void KIFImageList::slotSlideShow()
{
    if(showTimer){
        delete showTimer;
        showTimer = NULL;
        fileMnu->changeItem(slideShowID, i18n("Slide Show..."));
    }
    else{
        KIFSlideDialog dlg(this);
        if(dlg.exec() == QDialog::Accepted && fileList->count() != 0){
            fileMnu->changeItem(slideShowID, i18n("Stop Slide Show"));
            if(showTimer)
                delete showTimer;
            showTimer = new QTimer;
            connect(showTimer, SIGNAL(timeout()), SLOT(slotSlideShowTimer()));
            showTimer->start(dlg.delay()*1000);
        }
    }
}

void KIFImageList::slotSlideShowTimer()
{
    unsigned int i = fileList->currentItem();
    if(i == fileList->count()-1)
        i = 0;
    else
        ++i;
    fileList->setCurrentItem(i);
    imageWindow->slotSetFile(((KIFImageListItem *)fileList->
                              item(i))->fileName());
}

void KIFImageList::slotScaleIn()
{
    int pW = imageWindow->width() + (int)(imageWindow->width()*0.1);
    int pH = imageWindow->height() + (int)(imageWindow->height()*0.1);
    imageWindow->resize(pW, pH);
}

void KIFImageList::slotScaleOut()
{
    int pW = imageWindow->width() - (int)(imageWindow->width()*0.1);
    int pH = imageWindow->height() - (int)(imageWindow->height()*0.1);
    imageWindow->resize(pW, pH);
}

void KIFImageList::slotDouble()
{
    imageWindow->resize(imageWindow->width()+imageWindow->width(),
                        imageWindow->height()+imageWindow->height());
}

void KIFImageList::slotShrink()
{
    if(imageWindow->width() > 1 && imageWindow->height() > 1){
        int pW = (int)(imageWindow->width()*0.5);
        int pH = (int)(imageWindow->height()*0.5);
        imageWindow->resize(pW, pH);
    }
}

void KIFImageList::slotBack()
{
    if(fileList->count() > 1){
        int currentItem = fileList->currentItem();
        if(currentItem-1 >= 0){
            fileList->setCurrentItem(currentItem-1);
            imageWindow->slotSetFile(((KIFImageListItem *)fileList->
                                      item(currentItem-1))->fileName());
        }
    }
}

void KIFImageList::slotForward()
{
    if(fileList->count() > 1){
        unsigned int currentItem = fileList->currentItem();
        if(currentItem+1 < fileList->count()){
            fileList->setCurrentItem(currentItem+1);
            imageWindow->slotSetFile(((KIFImageListItem *)fileList->
                                      item(currentItem+1))->fileName());
        }
    }
}

void KIFImageList::slotReload()
{
    int currentItem = fileList->currentItem();
    if(currentItem != -1){
        imageWindow->slotSetFile(((KIFImageListItem *)fileList->
                                  item(currentItem))->fileName());
    }
}

void KIFImageList::slotBright()
{
    QApplication::setOverrideCursor(kifapp()->busyCursor1());
    QImage *image = imageWindow->image();
    KImageEffect::intensity(*image, .1);
    imageWindow->slotUpdateFromImage();
    QApplication::restoreOverrideCursor();
}

void KIFImageList::slotDim()
{
    QApplication::setOverrideCursor(kifapp()->busyCursor1());
    QImage *image = imageWindow->image();
    KImageEffect::intensity(*image, -.1);
    imageWindow->slotUpdateFromImage();
    QApplication::restoreOverrideCursor();
}

// FIXME
void KIFImageList::slotRotate45()
{
    kifapp()->startBusyCursor();
    QImage *image = imageWindow->image();
    QImage newImage(image->height(), image->width(), 32);
    unsigned int *srcData;
    int x, y;
    for(y=0; y < image->height(); ++y){
        srcData = (unsigned int *)image->scanLine(y);
        for(x=0; x < image->width(); ++x){
            newImage.setPixel(x, y, srcData[x]);
        }
    }
    *image = newImage;
    imageWindow->slotUpdateFromImage();
    kifapp()->stopBusyCursor();
}

void KIFImageList::slotIncContrast()
{
    kifapp()->startBusyCursor();
    QImage *image = imageWindow->image();
    int startx, starty, endx, endy;
    QRect *selRect = imageWindow->selectRect();
    if(selRect->isValid()){
        startx  = selRect->x();
        endx = selRect->right();
        starty = selRect->y();
        endy = selRect->bottom();
    }
    else{
        startx = 0, starty = 0;
        endx = image->width()-1;
        endy = image->height()-1;
    }
    unsigned int *data;
    int x, y, cursorCount=0;
    QRgb r, g, b;
    for(y=starty; y <= endy; ++y){
        if(cursorCount == 5){
            cursorCount = 0;
            kifapp()->incBusyCursor();
        }
        else
            ++cursorCount;
        data = (unsigned int *)image->scanLine(y);
        for(x=startx; x <= endx; ++x){
            r = qRed(data[x]);
            g = qGreen(data[x]);
            b = qBlue(data[x]);
            if(qGray(data[x]) <= 127){
                if(r - 4 <= 255)
                    r -= 4;
                if(g - 4 <= 255)
                    g -= 4;
                if(b - 4 <= 255)
                    b -= 4;
            }
            else{
                if(r + 4 <= 255)
                    r += 4;
                if(g + 4 <= 255)
                    g += 4;
                if(b + 4 <= 255)
                    b += 4;
            }
            data[x] = qRgb(r, g, b);
        }
    }
    imageWindow->slotUpdateFromImage();
    kifapp()->stopBusyCursor();
}

void KIFImageList::slotDecContrast()
{
    kifapp()->startBusyCursor();
    QImage *image = imageWindow->image();
    int startx, starty, endx, endy;
    QRect *selRect = imageWindow->selectRect();
    if(selRect->isValid()){
        startx  = selRect->x();
        endx = selRect->right();
        starty = selRect->y();
        endy = selRect->bottom();
    }
    else{
        startx = 0, starty = 0;
        endx = image->width()-1;
        endy = image->height()-1;
    }
    unsigned int *data;
    int x, y, cursorCount=0;
    QRgb r, g, b;
    for(y=starty; y <= endy; ++y){
        if(cursorCount == 5){
            cursorCount = 0;
            kifapp()->incBusyCursor();
        }
        else
            ++cursorCount;
        data = (unsigned int *)image->scanLine(y);
        for(x=startx; x <= endx; ++x){
            r = qRed(data[x]);
            g = qGreen(data[x]);
            b = qBlue(data[x]);
            if(qGray(data[x]) <= 127){
                if(r + 4 <= 255)
                    r += 4;
                if(g + 4 <= 255)
                    g += 4;
                if(b + 4 <= 255)
                    b += 4;
            }
            else{
                if(r - 4 <= 255)
                    r -= 4;
                if(g - 4 <= 255)
                    g -= 4;
                if(b - 4 <= 255)
                    b -= 4;
            }
            data[x] = qRgb(r, g, b);
        }
    }
    imageWindow->slotUpdateFromImage();
    kifapp()->stopBusyCursor();
}

void KIFImageList::slotMirrorHoriz()
{
    QApplication::setOverrideCursor(kifapp()->busyCursor1());
    QImage *image = imageWindow->image();
    *image = image->mirror(true, false);
    imageWindow->slotUpdateFromImage();
    QApplication::restoreOverrideCursor();
}

void KIFImageList::slotMirrorVert()
{
    QApplication::setOverrideCursor(kifapp()->busyCursor1());
    QImage *image = imageWindow->image();
    *image = image->mirror();
    imageWindow->slotUpdateFromImage();
    QApplication::restoreOverrideCursor();
}

void KIFImageList::slotInvert()
{
    QApplication::setOverrideCursor(kifapp()->busyCursor1());
    QImage *image = imageWindow->image();
    image->invertPixels(false);
    imageWindow->slotUpdateFromImage();
    QApplication::restoreOverrideCursor();
}

void KIFImageList::slotCrop()
{
    QRect *r = imageWindow->selectRect();
    if(!r->isValid()){
        KMessageBox::error(this, i18n("No rectangle selected!"));
        return;
    }
    kifapp()->startBusyCursor();
    QImage *image = imageWindow->image();
    QImage origImage(*image);
    origImage.detach();
    image->reset();
    image->create(r->width(), r->height(), 32);
    int ix, ox, iy, oy, cursorCount = 0;
    unsigned int *input, *output;
    for(oy=0, iy = r->y(); oy < r->height(); ++iy, ++oy){
        if(cursorCount == 5){
            cursorCount = 0;
            kifapp()->incBusyCursor();
        }
        else
            ++cursorCount;
        input = (unsigned int *)origImage.scanLine(iy);
        output = (unsigned int *)image->scanLine(oy);
        for(ox=0, ix = r->x(); ox < r->width(); ++ix, ++ox)
            output[ox] = input[ix];
    }
    imageWindow->slotUpdateFromImage();
    kifapp()->stopBusyCursor();
}

void KIFImageList::slotImageColor()
{
    if(!imageColorDlg)
        loadColorPlugin();
    if(imageColorDlg)
        imageColorDlg();

}

void KIFImageList::slotSingleColor()
{
    if(!singleColorDlg)
        loadColorPlugin();
    if(singleColorDlg)
        singleColorDlg();
}

void KIFImageList::slotGrab()
{
    KIFScreenGrab dlg(this, this);
    //dlg.exec();
}

void KIFImageList::slotGenerateHtml()
{
    KIFHtmlExport dlg(fileList, this);
    dlg.exec();
}

void KIFImageList::slotDropFinished()
{
    slotURLSelected(fileList->item(fileList->count()-1));
    fileList->setCurrentItem(fileList->count()-1);
}

void KIFImageList::slotPlayAni()
{
    int currentItem = fileList->currentItem();
    if(currentItem != -1){
        MovieWindow *w = new MovieWindow(((KIFImageListItem *)fileList->
                                          item(currentItem))->fileName());
        w->resize(imageWindow->size());
    }
}

void KIFImageList::closeEvent(QCloseEvent *ev)
{
    hide();
    ev->ignore();
}

int KIFImageList::findURLIndex(const QString &url)
{
    unsigned int i;
    for(i=0; i < fileList->count(); ++i){
        if(fileList->text(i) == url)
            return(i);
    }
    return(-1);
}

void KIFImageList::slotHide()
{
    wasVisible = isVisible();
    hide();
}

void KIFImageList::slotShow()
{
    if(wasVisible)
        show();
}

void KIFImageList::slotQuit()
{
    imageWindow->close();
}

void KIFImageList::keyPressEvent(QKeyEvent *ev)
{
    // first, see if it's a custom key
    if(ev->key() == keys->currentKey("Double"))
        slotDouble();
    else if(ev->key() == keys->currentKey("Half"))
        slotShrink();
    else if(ev->key() == keys->currentKey("Brighten"))
        slotBright();
    else if(ev->key() == keys->currentKey("Dim"))
        slotDim();
    else // next see if it's a std menu accelerator
        KTMainWindow::keyPressEvent(ev);
}

#include "imagelist.moc"

