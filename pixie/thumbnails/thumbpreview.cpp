#include "thumbpreview.h"
#include <qpainter.h>
#include <qapp.h>
#include <klocale.h>

KIFThumbPreview::KIFThumbPreview(QWidget *parent, const char *name)
    : QWidget(parent, name)
{
    loading = false;
    err = false;
    QFont tmp = font();
    tmp.setBold(true);
    setFont(tmp);
}

void KIFThumbPreview::enabledChange(bool)
{
    if(!isEnabled()){
        previewPix.resize(0, 0);
        repaint(false);
    }
}

QSize KIFThumbPreview::sizeHint() const
{
    return(QSize(150, 150));
}

void KIFThumbPreview::setFileName(const QString &fileStr)
{
    if(isEnabled()){
        loading = true;
        err = !previewPix.load(fileStr);
        loading = false;
        imgPos.setX(0);
        imgPos.setY(0);
        repaint(false);
    }
}

void KIFThumbPreview::paintEvent(QPaintEvent *ev)
{
    QRect r = ev->rect();
    QPainter p(this);
    if(isEnabled()){
        if(loading){
            p.fillRect(rect(), Qt::white);
            p.setPen(Qt::blue);
            p.drawText(rect(), AlignCenter, i18n("Loading..."));
        }
        else if(err){
            p.fillRect(rect(), Qt::white);
            p.setPen(Qt::red);
            p.drawText(rect(), AlignCenter, i18n("Unable to load image."));
        }
        else{
            if(previewPix.isNull() || r.bottom() > previewPix.height()-1 ||
               r.right() > previewPix.width()-1)
                p.fillRect(r, Qt::gray);
            if(!previewPix.isNull())
                p.drawPixmap(0, 0, previewPix, imgPos.x(), imgPos.y());
        }
    }
    else
        p.fillRect(r, Qt::gray);
        
}

void KIFThumbPreview::mousePressEvent(QMouseEvent *ev)
{
    mPos = ev->pos();
    QApplication::setOverrideCursor(sizeAllCursor);
}

void KIFThumbPreview::mouseMoveEvent(QMouseEvent *ev)
{
    if(ev->pos() != mPos){
        if(previewPix.width() > width()){
            if(ev->pos().x() > mPos.x())
                imgPos.setX(imgPos.x() + (ev->pos().x() - mPos.x()));
            else if(ev->pos().x() < mPos.x())
                imgPos.setX(imgPos.x() - (mPos.x() - ev->pos().x()));
            if(imgPos.x() < 0)
                imgPos.setX(0);
            if(imgPos.x() > previewPix.width()-width())
                imgPos.setX(previewPix.width()-width());
        }

        if(previewPix.height() > height()){
            if(ev->pos().y() > mPos.y())
                imgPos.setY(imgPos.y() + (ev->pos().y() - mPos.y()));
            else if(ev->pos().y() < mPos.y())
                imgPos.setY(imgPos.y() - (mPos.y() - ev->pos().y()));
            if(imgPos.y() < 0)
                imgPos.setY(0);
            if(imgPos.y() > previewPix.height()-height())
                imgPos.setY(previewPix.height()-height());
        
        }
        
        repaint(false);
    }
}
    
void KIFThumbPreview::mouseReleaseEvent(QMouseEvent *)
{
    QApplication::restoreOverrideCursor();
}

