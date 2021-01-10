#include "bigwindow.h"
#include "imagewindow.h"
#include "ifapp.h"
#include <qscrollview.h>
#include <kstatusbar.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <klocale.h>

KIFBigView::KIFBigView(QWidget *parent, const char *name)
    : QWidget(parent, name)
{
    pix = NULL;
    inMouseMove = false;
}

void KIFBigView::setPixmap(QPixmap &p)
{
    pix = &p;
    if(p.width()*4 != width() || p.height()*4 != height())
        resize(p.width()*4, p.height()*4);
    else
        repaint(false);
}

void KIFBigView::setSelection(const QRect &r)
{
    QRect oldRect = selRect;
    selRect = r;
    if(oldRect.isValid())
        repaint(oldRect.x()*4, oldRect.y()*4, oldRect.width()*4,
                oldRect.height()*4, false);
    if(selRect.isValid())
        repaint(selRect.x()*4, selRect.y()*4, selRect.width()*4,
                selRect.height()*4, false);
}

void KIFBigView::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == QMouseEvent::LeftButton){
        selMouseX = ev->x()/4;
        selMouseY = ev->y()/4;
        inMouseMove = true;
        selPainter.begin(this);
        selPainter.scale(4, 4);
        selPainter.setPen(Qt::black);
        selPainter.setRasterOp(NotROP);
    }
    else
        emit pointClicked(ev->x()/4, ev->y()/4);
}

void KIFBigView::mouseMoveEvent(QMouseEvent *ev)
{
    if(inMouseMove){
        int x = ev->x()/4;
        int y = ev->y()/4;
        if(x != selMouseX && y != selMouseY){
            if(selRect.isValid()){
                selPainter.drawRect(selRect);
            }
            if(x > selMouseX){
                selRect.setX(selMouseX);
                selRect.setRight(x);
            }
            else{
                selRect.setX(x);
                selRect.setRight(selMouseX);
            }
            if(y > selMouseY){
                selRect.setY(selMouseY);
                selRect.setBottom(y);
            }
            else{
                selRect.setY(y);
                selRect.setBottom(selMouseY);
            }
            if(selRect.isValid()){
                selPainter.drawRect(selRect);
            }
        }
    }
}

void KIFBigView::mouseReleaseEvent(QMouseEvent *)
{
    if(inMouseMove){
        inMouseMove = false;
        selPainter.end();
        emit selectionChanged(selRect);
    }

}

void KIFBigView::mouseDoubleClickEvent(QMouseEvent *)
{
    if(inMouseMove){
        inMouseMove = false;
        selPainter.end();
    }
    if(selRect.isValid()){
        QRect oldRect = selRect;
        selRect.setWidth(0);
        if(oldRect.isValid())
            repaint(oldRect.x()*4, oldRect.y()*4, oldRect.width()*4,
                    oldRect.height()*4, false);
    }
    emit selectionChanged(selRect);
}

void KIFBigView::paintEvent(QPaintEvent *ev)
{
    if(pix){
        QPainter p(this);
        p.scale(4, 4);
        QRect r = p.xFormDev(ev->rect());
        // no idea why I have to do this but if I don't the area doesn't get
        // entirely filled :P Using ev->rect() for the pixmap coords doesn't work
        // either, although that is not scaled...
        if(r.x() > 0)
            r.setX(r.x()-1);
        if(r.y() > 0)
            r.setY(r.y()-1);
        r.setWidth(r.width()+2);
        r.setHeight(r.height()+2);
        p.drawPixmap(r.topLeft(), *pix, r);
        if(selRect.isValid()){
            p.setRasterOp(NotROP);
            p.setPen(Qt::black);
            p.drawRect(selRect);
            p.end();
        }
    }
}

KIFBigTopLevel::KIFBigTopLevel(const char *name)
   : QVBox(NULL, name)

{
    win = NULL;
    QScrollView *scroll = new QScrollView(this);
    view = new KIFBigView(scroll->viewport());
    scroll->addChild(view);
    connect(view, SIGNAL(pointClicked(int, int)), this,
            SLOT(slotPointClicked(int, int)));
    connect(view, SIGNAL(selectionChanged(const QRect &)), this,
            SLOT(slotSelection(const QRect &)));
    status = new KStatusBar(this);
    status->insertItem(i18n("KIF Big View: LMB=Select, RMB=Color Lookup"), 0);
    setCaption(i18n("Big Bits View"));
    connect(kifapp(), SIGNAL(hideAllWindows()), this, SLOT(slotHide()));
    connect(kifapp(), SIGNAL(showAllWindows()), this, SLOT(slotShow()));
}

void KIFBigTopLevel::slotSetPixmap(QPixmap &p)
{
    view->setPixmap(p);
}

void KIFBigTopLevel::slotSetSelection(const QRect &r)
{
    view->setSelection(r);
}

void KIFBigTopLevel::slotPointClicked(int x, int y)
{
    if(win){
        QColor color(win->color(x, y));
        QString coordStr;
        coordStr.sprintf("(%d, %d)", x, y);
        QString decStr;
        decStr.sprintf("%d,%d,%d", color.red(), color.green(), color.blue());
        status->changeItem(i18n("Color at ") + coordStr + i18n(" Decimal: ") + decStr + i18n(" Hex: ") +
                           color.name(), 0);
    }
}

void KIFBigTopLevel::slotSelection(const QRect &r)
{
    emit selectionChanged(r);
}


void KIFBigTopLevel::closeEvent(QCloseEvent *)
{
    emit bigBitsClosed();
    delete this;
}

void KIFBigTopLevel::slotHide()
{
    wasVisible = isVisible();
    hide();
}

void KIFBigTopLevel::slotShow()
{
    if(wasVisible)
        show();
}



#include "bigwindow.moc"


    


