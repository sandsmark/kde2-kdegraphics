#ifndef __THUMB_PREVIEW_H
#define __THUMB_PREVIEW_H

#include <qwidget.h>
#include <qpixmap.h>

class KIFThumbPreview : public QWidget
{
public:
    KIFThumbPreview(QWidget *parent=0, const char *name=0);
    void setFileName(const QString &fileStr);
    QSize sizeHint() const;
    QPixmap& pixmap(){return(previewPix);}
    bool isError(){return(err);}
protected:
    void paintEvent(QPaintEvent *ev);
    void enabledChange(bool old);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    QPixmap previewPix;
    bool loading, err;
    QPoint mPos;
    QPoint imgPos;
};


#endif


