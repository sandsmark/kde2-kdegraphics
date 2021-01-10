#ifndef __KIF_BIGWINDOW_H
#define __KIF_BIGWINDOW_H

#include <qvbox.h>
#include <qpainter.h>
class QPixmap;
class KStatusBar;
class KIFBigView;
class KIFImageWindow;

class KIFBigTopLevel : public QVBox
{
    Q_OBJECT
public:
    KIFBigTopLevel(const char *name=0);
    void setImageWindow(KIFImageWindow *w){win = w;}
public slots:
    void slotSetPixmap(QPixmap &p);
    void slotSetSelection(const QRect &r);
signals:
    void selectionChanged(const QRect &r);
    void bigBitsClosed();
protected slots:
    void slotPointClicked(int x, int y);
    void slotSelection(const QRect &r);
    void slotHide();
    void slotShow();
protected:
    void closeEvent(QCloseEvent *ev);

    bool wasVisible;
    KIFBigView *view;
    KStatusBar *status;
    KIFImageWindow *win;
};

// internal scrolled view for use in KIFBigTopLevel
class KIFBigView : public QWidget
{
    Q_OBJECT
public:
    KIFBigView(QWidget *parent=0, const char *name=0);
    void setPixmap(QPixmap &p);
    void setSelection(const QRect &r);
signals:
    void pointClicked(int x, int y);
    void selectionChanged(const QRect &r);
protected:
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *ev);

    QPixmap *pix;
    QPainter selPainter;
    int selMouseX, selMouseY;
    bool inMouseMove;
    QRect selRect;
};



#endif
