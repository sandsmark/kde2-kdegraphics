// $Id: roundangle.cpp 72077 2000-11-25 10:06:51Z mlaurent $

#include <kdebug.h>
#include <stdio.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qstring.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "roundangle.h"
#include <kiconloader.h>

#define ROUNDNESS (40)

Roundangle::Roundangle(const QString & toolname) : Tool(toolname)
{
  drawing= FALSE;

  tooltip= i18n("Roundangle");
  props= Tool::HasLineProperties | Tool::HasFillProperties;
}

void Roundangle::activating()
{
kdDebug(4400) << "Roundangle::activating() hook called\n" << endl;
  canvas->setCursor(crossCursor);
}

void Roundangle::mousePressEvent(QMouseEvent *e)
{
kdDebug(4400) << "Roundangle::mousePressEvent() handler called\n" << endl;
  
  if (isActive()) {
    if (drawing) {
      kdDebug(4400) << "RubberLine: Warning Left Button press received when pressed\n" << endl;
    }
    else {
      startx= (e->pos()).x();
      starty= (e->pos()).y();
      activeButton= e->button();
      lastx= startx;
      lasty= starty;
      drawing= TRUE;
    } 
  }
  if (!isActive()) {
kdWarning(4400) << "Warning event received when inactive (ignoring)\n" << endl;
  }
}

void Roundangle::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	paint.begin(canvas->zoomedPixmap());

	if (activeButton == LeftButton)
	  paint.setPen(leftpen);
	else
	  paint.setPen(rightpen);

	paint.setRasterOp(DEFAULT_RASTER_OP);

	// Erase old roundangle
	paint.drawRoundRect(startx, starty, lastx-startx, lasty-starty, 
			    ROUNDNESS, ROUNDNESS);
	// Draw new roundangle
	paint.drawRoundRect(startx, starty, x-startx, y-starty,
			    ROUNDNESS, ROUNDNESS);

	lastx= x;
	lasty= y;

	paint.end();
	canvas->repaint(0);
      }
    }
  }
  else {
kdWarning(4400) << "Warning event received when inactive (ignoring)\n" << endl;
  }
}

void Roundangle::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  QWMatrix m;

kdDebug(4400) << "Roundangle::mouseReleaseEvent() handler called\n" << endl;

  if (isActive() && (e->button() == activeButton)) {
    canvas->markModified();
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old line
    paint.begin(canvas->zoomedPixmap());

    if (activeButton == LeftButton)
      paint.setPen(leftpen);
    else
      paint.setPen(rightpen);

    paint.setRasterOp(DEFAULT_RASTER_OP);
    paint.drawRoundRect(startx, starty, lastx-startx, lasty-starty,
			ROUNDNESS, ROUNDNESS);

    paint.end();

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    paint.begin(canvas->pixmap());

    if (activeButton == LeftButton) {
      paint.setPen(leftpen);
      paint.setBrush(leftbrush);
    }
    else {
      paint.setPen(rightpen);
      paint.setBrush(rightbrush);
    }

    paint.setWorldMatrix(m);
    paint.setRasterOp(CopyROP);
    // Draw new roundangle
    paint.drawRoundRect(startx, starty, x-startx, y-starty, ROUNDNESS, ROUNDNESS);
    paint.end();
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);
  }
  else {
kdWarning(4400) << "Warning event received when inactive (ignoring)\n" << endl;
  }
}

QPixmap Roundangle::pixmap() const
{
    return UserIcon("roundangle");
}

#include "roundangle.moc"

