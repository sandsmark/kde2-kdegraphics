#ifndef __IMAGEWINDOW_H
#define __IMAGEWINDOW_H

#include <qwidget.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpainter.h>

class KIFBigTopLevel;

/*
 Optimized window for images expected to be edited or converted. There
 are two main data items in here that deserve mention, the image (with the
 local client image bits), and the pixmap on the X server being displayed. When
 an image is first loaded we check to see if the display depth is 32bpp. If
 it is we just load it into the pixmap on the server and reset the client
 image to NULL. If we need to access it's bits we can convert later and not
 loose data, thus saving memory. If the display depth is 8bpp we load to the
 client image using 32bpp and then dither the pixmap for display so we don't
 loose colors internally. If the display is 16bpp you can configure this
 either way, with the default being to use the image. It doesn't matter what
 the actual image depth is, we only pay attention to the display.

 Another brief mention to scaling. We smoothscale the image when the window
 is resized. Doing multiple smoothscales to the same image results in
 exessive blurring, so we smoothscale from the original image to a temporary
 image. When image() is called the scaled image is then converted to the
 main one and released.
 */

class KIFImageWindow : public QWidget
{
    Q_OBJECT
public:
    KIFImageWindow(QWidget *parent=0, const char *name=0);
    bool load(const QString &filename);
    QImage* image();
    QPixmap* pixmap(){return(&pix);}
    QRect* selectRect(){return(&selRect);}
    QRgb color(int x, int y);
public slots:
    void slotSetFile(const QString &fileStr);
    void slotSetSelection(const QRect &r);
    void slotUpdateFromImage();
    void slotUpdatePixmap();
    void slotBigBitsClosed();
protected slots:
    void slotHide();
    void slotShow();
signals:
    void toggleFileList();
    void invalidFile();
    void selectionChanged(const QRect &r);
    void urlDropped(const QString &urlStr);
    void dropFinished();
protected:
    void drawLogoText();
    void dragEnterEvent(QDragEnterEvent *ev);
    void dropEvent(QDropEvent *ev);
    void closeEvent(QCloseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void resizeEvent(QResizeEvent *ev);
    void paintEvent(QPaintEvent *ev);
    void resizeWithMaxpect();

    QPixmap pix;
    QImage pixImage;
    QImage scaleImage;
    QRect selRect;
    int selMouseX, selMouseY;
    int frameW, frameH;
    bool selXORFlag;
    QPainter selPainter;
    bool inMouseMove, cache16bpp, wasVisible, maxpect;
    KIFBigTopLevel *bigBitWindow;
};
#endif
