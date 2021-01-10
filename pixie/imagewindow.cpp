#include "imagewindow.h"
#include "ifapp.h"
#include "imagelist.h"
#include "bigwindow.h"
#include <kiconloader.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kwin.h>
#include <kurl.h>
#include <klocale.h>
#include <qdragobject.h>
#include <qwmatrix.h>

KIFImageWindow *pluginHackWinPtr; // hack, used in pluginmenu bindings

void scale(QImage &origImage, QImage &scaledImage)
{
    double perX = ((double)origImage.width())/(double)scaledImage.width();
    double perY = ((double)origImage.height())/(double)scaledImage.height();
        
    qWarning("%f, %f", perX, perY);
    int origW = origImage.width();
    int x, y;
    unsigned int *p = (unsigned int *)scaledImage.bits();
    unsigned int *origP = (unsigned int *)origImage.bits();
    int sampX, sampY;
    for(y=0; y < scaledImage.height(); ++y){
        sampY = (int)(y * perY);
        for(x=0; x < scaledImage.width(); ++x, ++p){
            sampX = (int)(x * perX);
            *p  = origP[(sampY*origW)+sampX];
        }
    }
}
    

KIFImageWindow::KIFImageWindow(QWidget *parent, const char *name)
    : QWidget(parent, name/* ,WResizeNoErase*/)
{
    setAcceptDrops(true);
    setFocusPolicy(ClickFocus);
    qWarning("Display depth: %d", QPixmap::defaultDepth());
    bigBitWindow = NULL;
    KConfig *config = KGlobal::config();
    config->setGroup("ImageWindow");
    cache16bpp = config->readBoolEntry("Promote16bpp", true);
    inMouseMove = false;
    maxpect = false; // hack to avoid trying to maxpect until framed
    slotSetFile(KGlobal::dirs()->findResource("appdata", "logo.png"));
    drawLogoText();

    connect(kifapp(), SIGNAL(hideAllWindows()), this, SLOT(slotHide()));
    connect(kifapp(), SIGNAL(showAllWindows()), this, SLOT(slotShow()));

    pluginHackWinPtr = this;
    setBackgroundMode(NoBackground);
    pixImage.setAlphaBuffer(true);
    scaleImage.setAlphaBuffer(true);
    show();
    // calculate window frame & titlebar size. Logo is 340x226.
    KWin::Info info = KWin::info(winId());
    QRect fR = info.frameGeometry;
    frameW = fR.width()-340;
    frameH = fR.height()-226;
    qWarning("Frame: %dx%d", frameW, frameH);
    maxpect = true;
}

void KIFImageWindow::drawLogoText()
{
    QPainter p(&pix);
    QFont f = font();
    f.setBold(true);
    f.setPointSize(12);
    p.setFont(f);
    int i = QFontMetrics(f).height()+1;
    p.drawText(142, 20, i18n("(C)Daniel M. Duley"));
    p.drawText(142, 20+i, i18n("<mosfet@mandrakesoft.com>"));
    p.drawText(142, 20+i*2, i18n("<mosfet@kde.org>"));
    p.drawText(142, 20+i*3, i18n("Logo by <kris@animexx.de>"));
    p.drawText(150, 25+i*4, i18n("Mouse controls:"));
    p.drawText(150, 25+i*5, i18n("Left: Select Area"));
    p.drawText(150, 25+i*6, i18n("Middle: Lookup color"));
    p.drawText(150, 25+i*7, i18n("Right: Menu/Controls"));
    p.drawText(150, 25+i*8, i18n("Dbl Click: Unselect"));

    p.end();
}


QImage* KIFImageWindow::image()
{

    if(!scaleImage.isNull()){
        pixImage = scaleImage;
        pixImage.detach();
        scaleImage.reset();
    }
    else if(pixImage.isNull()){
        if(QPixmap::defaultDepth() < 32) // remove this
            qWarning("No pixImage for display < 32bpp!");

        // we are on 32bpp and delayed creating an image
        pixImage = pix.convertToImage();
    }
    return(&pixImage);
}

QRgb KIFImageWindow::color(int x, int y)
{
    QImage *img = image();
    if(x > img->width()-1 || y > img->height()-1){
        qWarning("KIF error: KIFImageWindow::color called with invalid coords!");
        return(qRgb(0, 0, 0));
    }
    QRgb *data = (QRgb *)img->scanLine(y);
    return(data[x]);
}
        
void KIFImageWindow::slotSetSelection(const QRect &r)
{
    QRect oldSelRect = selRect;
    selRect.setWidth(0);
    repaint(oldSelRect);
    selRect = r;
    repaint(selRect);
}


void KIFImageWindow::resizeWithMaxpect()
{
    if(!maxpect){
        resize(pix.size());
        return;
    }
    if(pix.width() != width() || pix.height() != height()){
        QRect deskRect = QApplication::desktop()->rect();
        deskRect.setWidth(deskRect.width()-frameW-10);
        deskRect.setHeight(deskRect.height()-frameH-10);
        QRect r = QRect(x(), y(), pix.width(), pix.height());
        // may need to do this twice
        if(r.width() > deskRect.width() || r.height() > deskRect.height()){
            while(r.width() > deskRect.width() || r.height() > deskRect.height()){
                if(r.width() > deskRect.width()){
                    float percent = ((float)deskRect.width())/r.width();
                    r.setWidth((int)r.width()*percent);
                    r.setHeight((int)r.height()*percent);
                    qWarning("Scaling width");
                }
                if(r.height() > deskRect.height()){
                    float percent = ((float)deskRect.height())/r.height();
                    r.setWidth((int)r.width()*percent);
                    r.setHeight((int)r.height()*percent);
                    qWarning("Scaling height");
                }
            }
            // reposition if needed
            if(r.bottom() > deskRect.bottom())
                r.setTop(deskRect.top());
            if(r.right() > deskRect.right())
                r.setLeft(deskRect.left());
        }
        resize(r.size());
    }
    else
        repaint(false);
}



void KIFImageWindow::slotUpdateFromImage()
{
    pix.convertFromImage(pixImage);
    resizeWithMaxpect();
    if(bigBitWindow)
        bigBitWindow->slotSetPixmap(pix);
}

void KIFImageWindow::slotUpdatePixmap()
{
    pixImage.reset();
    scaleImage.reset();
    selRect.setWidth(0);
    emit selectionChanged(selRect);
    resizeWithMaxpect();
}

void KIFImageWindow::slotSetFile(const QString &fileStr)
{
    QApplication::setOverrideCursor(waitCursor);
    pixImage.reset();
    scaleImage.reset();
    selRect.setWidth(0);

    if(QPixmap::defaultDepth() <= 8  ||
       (QPixmap::defaultDepth() <= 32 && cache16bpp)){
        // we need to initally load as an image to preserve all colors, and
        // then use a pixmap dithered to the display depth to draw
        pixImage.load(fileStr);
        if(pixImage.depth() < 32)
            pixImage = pixImage.convertDepth(32);
        pix.convertFromImage(pixImage);
    }
    else{
        // we are on a highcolor display so can initially use just a pixmap
        pix.load(fileStr);
        // unless that is if there is a alpha
        if(pix.mask()){
            pixImage.load(fileStr);
            if(pixImage.depth() < 32)
                pixImage = pixImage.convertDepth(32);
        }
    }

    if(pix.isNull()){
        pix.load(KGlobal::dirs()->findResource("appdata", "logo.png"));
        drawLogoText();
        emit invalidFile();
    }
    if(bigBitWindow)
        bigBitWindow->slotSetPixmap(pix);
    emit selectionChanged(selRect);
    QApplication::restoreOverrideCursor();
    resizeWithMaxpect();
}

void KIFImageWindow::resizeEvent(QResizeEvent *ev)
{

    bool updateBigBits = selRect.isValid();
    selRect.setWidth(0);
    if(ev->size() != pix.size()){
        if(pixImage.isNull())
            pixImage = pix.convertToImage();
        scaleImage = pixImage.smoothScale(ev->size().width(),
                                          ev->size().height());
        pix.convertFromImage(scaleImage);
        if(bigBitWindow)
            bigBitWindow->slotSetPixmap(pix);
    }
    if(updateBigBits)
        emit selectionChanged(selRect);
}

void KIFImageWindow::paintEvent(QPaintEvent *ev)
{

    QRect r = ev->rect();
    if(pix.mask()){
        QPainter p;
        p.begin(this);
        p.fillRect(r, Qt::black);
        p.end();
    }
    bitBlt(this, r.topLeft(), &pix, r, Qt::CopyROP);
    if(selRect.isValid()){
        QPainter p;
        p.begin(this);
        p.setRasterOp(NotROP);
        p.setPen(Qt::black);
        p.drawRect(selRect);
        p.end();
    }
}

void KIFImageWindow::closeEvent(QCloseEvent *)
{
    if(bigBitWindow)
        delete bigBitWindow;
    kapp->quit();
}

void KIFImageWindow::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == QMouseEvent::LeftButton){
        selMouseX = ev->x();
        selMouseY = ev->y();
        inMouseMove = true;
        selPainter.begin(this);
        selPainter.setPen(Qt::black);
        selPainter.setRasterOp(NotROP);
    }
    else if(ev->button() == QMouseEvent::RightButton)
        emit toggleFileList();
    else if(ev->button() == QMouseEvent::MidButton){
        if(!bigBitWindow){
            bigBitWindow = new KIFBigTopLevel();
            bigBitWindow->setImageWindow(this);
            bigBitWindow->slotSetPixmap(pix);
            bigBitWindow->show();
            connect(bigBitWindow, SIGNAL(bigBitsClosed()), this,
                    SLOT(slotBigBitsClosed()));
            connect(bigBitWindow, SIGNAL(selectionChanged(const QRect &)),
                    this, SLOT(slotSetSelection(const QRect &)));
            connect(this, SIGNAL(selectionChanged(const QRect &)),
                    bigBitWindow, SLOT(slotSetSelection(const QRect &)));
        }
        else{
            bigBitWindow->showNormal();
            bigBitWindow->raise();
        }
    }
}

void KIFImageWindow::mouseMoveEvent(QMouseEvent *ev)
{
    if(inMouseMove){
        if(ev->x() != selMouseX && ev->y() != selMouseY){
            if(selRect.isValid()){
                selPainter.drawRect(selRect);
            }
            if(ev->x() > selMouseX){
                selRect.setX(selMouseX);
                selRect.setRight(ev->x());
            }
            else{
                selRect.setX(ev->x());
                selRect.setRight(selMouseX);
            }
            if(ev->y() > selMouseY){
                selRect.setY(selMouseY);
                selRect.setBottom(ev->y());
            }
            else{
                selRect.setY(ev->y());
                selRect.setBottom(selMouseY);
            }
            if(selRect.isValid()){
                selPainter.drawRect(selRect);
            }
        }
    }
}

void KIFImageWindow::mouseReleaseEvent(QMouseEvent *)
{
    if(inMouseMove){
        inMouseMove = false;
        selPainter.end();
        emit selectionChanged(selRect);
    }
}

void KIFImageWindow::mouseDoubleClickEvent(QMouseEvent *)
{
    if(inMouseMove){
        inMouseMove = false;
        selPainter.end();
    }
    if(selRect.isValid()){
        QRect oldSelRect = selRect;
        selRect.setWidth(0);
        repaint(oldSelRect);
    }
    emit selectionChanged(selRect);

}

void KIFImageWindow::slotBigBitsClosed()
{
    qWarning("Big Bits window closed");
    bigBitWindow = NULL;
}

void KIFImageWindow::dragEnterEvent(QDragEnterEvent *ev)
{
    ev->accept(QUriDrag::canDecode(ev));
}

void KIFImageWindow::dropEvent(QDropEvent *ev)
{
    QStrList fileList;
    bool validUrls = false;
    if(QUriDrag::decode(ev, fileList)){
        QStrListIterator it(fileList);
        for(;it.current(); ++it){
            qWarning("In dropEvent for %s", it.current());
            KURL url(it.current());
            if(!url.isMalformed()){
                validUrls = true;
                emit urlDropped(it.current());
            }
        }
        if(validUrls)
            emit dropFinished();
    }
}

void KIFImageWindow::slotHide()
{
    wasVisible = isVisible();
    hide();
}

void KIFImageWindow::slotShow()
{
    if(wasVisible)
        show();
}





#include "imagewindow.moc"
