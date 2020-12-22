/****************************************************************************
**
** A simple widget to control scrolling in two dimensions.
**
** Copyright (C) 1997 by Markku Hihnala. 
** This class is freely distributable under the GNU Public License.
**
*****************************************************************************/


#include <qdrawutil.h>
#include <qpainter.h>

#include "scrollbox.h"

ScrollBox::ScrollBox( QWidget * parent , const char * name )
    : QFrame( parent, name )
{
	setFrameStyle( Panel | Sunken );
	QFrame::setBackgroundMode(PaletteMid);
}

void ScrollBox::mousePressEvent ( QMouseEvent *e )
{
	mouse = e->pos();
	if ( e->button() == RightButton )
		emit button3Pressed();
	if ( e->button() == MidButton )
		emit button2Pressed();
	setCursor(Qt::sizeAllCursor);
}


void ScrollBox::mouseReleaseEvent(QMouseEvent *)
{
  setCursor(Qt::arrowCursor);
}


void ScrollBox::mouseMoveEvent ( QMouseEvent *e )
{
	if (e->state() != LeftButton)
		return;
	int dx = ( e->pos().x() - mouse.x() ) *
		 pagesize.width() /
		 width();
	int dy = ( e->pos().y() - mouse.y() ) * 
		 pagesize.height() /
		 height();
	
        QPoint newPos = viewpos + QPoint(dx, dy);
        if (newPos.x()+viewsize.width() > pagesize.width())
	  newPos.setX(pagesize.width() - viewsize.width());
	if (newPos.y()+viewsize.height() > pagesize.height())
          newPos.setY(pagesize.height() - viewsize.height());
        if (newPos.x() < 0)
          newPos.setX(0);
        if (newPos.y() < 0)
          newPos.setY(0);                                                               
 	setViewPos(newPos);

 	emit valueChanged(newPos);

	mouse = e->pos();
}

void ScrollBox::drawContents ( QPainter *p )
{
	if (pagesize.isEmpty())
          {
            p->eraseRect(0, 0, width(), height());
	    return;   
          }

	QRect c( contentsRect() );

	int len = pagesize.width();
	int x = c.x() + c.width() * viewpos.x() / len;
	int w = c.width() * viewsize.width() / len ;
	if ( w > c.width() ) w = c.width();

	len = pagesize.height();
	int y = c.y() + c.height() * viewpos.y() / len;
	int h = c.height() * viewsize.height() / len;
	if ( h > c.height() ) h = c.height();

        RasterOp rop = p->rasterOp();
        p->setRasterOp(NotROP);
	p->drawRect(x, y, w, h);
        p->setRasterOp(rop);
}

void ScrollBox::setPageSize( QSize s )
{
	pagesize = s;
	repaint();
}

void ScrollBox::setViewSize( QSize s )
{
	viewsize = s;
	repaint();
}

void ScrollBox::setViewPos( QPoint pos )
{
	if (viewpos == pos)
	  return;
        viewpos = pos;
        repaint();
}


#include "scrollbox.moc"
