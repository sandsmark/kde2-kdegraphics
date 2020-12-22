// $Id: pen.cpp 102068 2001-06-14 07:30:06Z waba $

#include <kdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <qcursor.h>
#include <qstring.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "pen.h"
#include <kiconloader.h>

Pen::Pen(const QString & toolname) : Tool(toolname)
{
  drawing= FALSE;
  tooltip= i18n("Pen");
  props= Tool::HasLineProperties;
}

void Pen::activating()
{
  kdDebug(4400) << "Pen::activating() hook called canvas=" << canvas << "\n" << endl;
  drawing= FALSE;

  canvas->setCursor(crossCursor);
}

void Pen::mousePressEvent(QMouseEvent *e)
{
  int x,y;
  QPainter painter1;
  QPainter painter2;
  QWMatrix m;

  kdDebug(4400) << "Pen::mousePressEvent() handler called\n" << endl;
  
  if (isActive()) {
    if (drawing) {
      kdDebug(4400) << "Pen: Warning button press received while drawing\n" << endl;
    }
    canvas->markModified();
    x= (e->pos()).x();
    y= (e->pos()).y();
    activeButton= e->button();

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));

    painter1.begin(canvas->pixmap());

    if (activeButton == LeftButton)
      painter1.setPen(leftpen);
    else
      painter1.setPen(rightpen);
    painter1.setWorldMatrix(m);

    painter2.begin(canvas->zoomedPixmap());

    if (activeButton == LeftButton)
      painter2.setPen(leftpen);
    else
      painter2.setPen(rightpen);

    painter1.drawPoint(x, y);
    painter2.drawPoint(x, y);

    painter1.end();
    painter2.end();

    canvas->repaint(0);
    lastx= x;
    lasty= y;
    drawing= TRUE;
  } 
  if (!isActive()) {
    kdWarning(4400) << "Warning event received when inactive (ignoring)\n" << endl;
  }
}


void Pen::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter painter1;
  QPainter painter2;
  QWMatrix m;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if (drawing) {
      if ((x != lastx) || (y != lasty)) {
	m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));

	painter1.begin(canvas->pixmap());

	if (activeButton == LeftButton)
	  painter1.setPen(leftpen);
	else
	  painter1.setPen(rightpen);

	painter1.setWorldMatrix(m);

	painter2.begin(canvas->zoomedPixmap());

	if (activeButton == LeftButton)
	  painter2.setPen(leftpen);
	else
	  painter2.setPen(rightpen);

	painter1.drawLine(lastx, lasty, x, y);
	painter2.drawLine(lastx, lasty, x, y);

	lastx= x;
	lasty= y;

	painter1.end();
	painter2.end();
	canvas->repaint(0);
      }
    }
  }
  else {
    kdWarning(4400) << "Warning event received when inactive (ignoring)\n" << endl;
  }
}

void Pen::mouseReleaseEvent(QMouseEvent *e)
{
  kdDebug(4400) << "Pen::mouseReleaseEvent() handler called\n" << endl;
  if (isActive() && (e->button() == activeButton) && drawing)
    drawing= FALSE;
  if (!isActive()) {
    kdWarning(4400) << "Warning event received when inactive (ignoring)\n" << endl;
  }
}

QPixmap Pen::pixmap() const
{
    return UserIcon("pen");
}

#include "pen.moc"
