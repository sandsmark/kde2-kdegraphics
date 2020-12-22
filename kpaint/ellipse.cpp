// $Id: ellipse.cpp 72077 2000-11-25 10:06:51Z mlaurent $

#include <kdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <qcursor.h>
#include <qstring.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "ellipse.h"
#include <kiconloader.h>

Ellipse::Ellipse() : Tool()
{
  drawing= FALSE;
  tooltip= i18n("Ellipse");
  props= Tool::HasLineProperties | Tool::HasFillProperties;
}

Ellipse::Ellipse(const QString & toolname) : Tool(toolname)
{
  drawing= FALSE;
  tooltip= i18n("Ellipse");
  props= Tool::HasLineProperties | Tool::HasFillProperties;
}

void Ellipse::activating()
{
kdDebug(4400) << "Ellipse::activating() hook called\n" << endl;
  canvas->setCursor(crossCursor);
}

void Ellipse::mousePressEvent(QMouseEvent *e)
{
kdDebug(4400) << "Ellipse::mousePressEvent() handler called\n" << endl;
  
  if (isActive()) {
    if (drawing) {
kdDebug(4400) << "Ellipse: Warning Left Button press received when pressed\n" << endl;
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

void Ellipse::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	// Erase old ellipse
	// The test is to prevent problems when the ellipse is smaller
	// than 2 by 2 pixels. (It leaves a point behind as the ellipse
	// grows).

	paint.begin(canvas->zoomedPixmap());

	if (activeButton == LeftButton)
	  paint.setPen(leftpen);
	else
	  paint.setPen(rightpen);

	paint.setRasterOp(DEFAULT_RASTER_OP);

	if (abs(lastx-startx)*abs(lasty-starty) >= 4) 
	  paint.drawEllipse(startx, starty, lastx-startx, lasty-starty);

	// Draw new ellipse
	if (abs(x-startx)*abs(y-starty) > 4) 
	  paint.drawEllipse(startx, starty, x-startx, y-starty);

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

void Ellipse::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  QWMatrix m;

kdDebug(4400) << "Ellipse::mouseReleaseEvent() handler called\n" << endl;

  if (isActive() && (e->button() == activeButton) && drawing) {
    canvas->markModified();
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old ellipse
    paint.begin(canvas->zoomedPixmap());

    if (activeButton == LeftButton)
      paint.setPen(leftpen);
    else
      paint.setPen(rightpen);

    paint.setRasterOp(DEFAULT_RASTER_OP);

    if (abs(lastx-startx)*abs(lasty-starty) > 4) 
      paint.drawEllipse(startx, starty, lastx-startx, lasty-starty);

    paint.end();

    paint.begin(canvas->pixmap());

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    paint.setWorldMatrix(m);
    paint.setRasterOp(CopyROP);

    // Draw new ellipse
    if (abs(x-startx)*abs(y-starty) > 4) {
      if (activeButton == LeftButton) {
	paint.setPen(leftpen);
	paint.setBrush(leftbrush);
      }
      else {
	paint.setPen(rightpen);
	paint.setBrush(rightbrush);
      }
      paint.drawEllipse(startx, starty, x-startx, y-starty);
    }

    paint.end();
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);
  }
  else {
kdWarning(4400) << "Warning event received when inactive (ignoring)\n" << endl;
  }
}

QPixmap Ellipse::pixmap() const
{
    return UserIcon("ellipse");
}

#include "ellipse.moc"
