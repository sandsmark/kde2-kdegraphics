// $Id: areaselect.cpp 74454 2000-12-11 19:53:29Z jcaliff $

#include <stdio.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qstring.h>
#include <qwmatrix.h>
#include <qclipboard.h>

#include <klocale.h>
#include <kapp.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kiconloader.h>

#include "areaselect.h"

#define TIMER_INTERVALL 500


AreaSelect::AreaSelect(const QString & toolname) : Tool(toolname)
{
  drawing= FALSE;
  showedSF= false;
  tooltip= i18n("Area Selection");
  props= 0;
}

/*
AreaSelect::~AreaSelect()
{
}
*/

void 
AreaSelect::selection(bool b)
{
  kdDebug() << "AreaSelect::selection " << b << endl;
  if (!b) {
    disableSelection();
  }
}

void 
AreaSelect::disableSelection()
{
  // stop all the timer 
  killTimers();
  if (showedSF) {
    drawFrame(); // to erase the frame
  }
}

void AreaSelect::activating()
{
  kdDebug(4400) << "AreaSelect::activating() hook called\n" << endl;
  canvas->setCursor(crossCursor);
}

void AreaSelect::deactivating()
{
  kdDebug(4400) << "AreaSelect::deactivating() hook called\n" << endl;
  // erase old selection frame
  disableSelection();
  canvas->clearSelection();
}

void
AreaSelect::timerEvent(QTimerEvent *)
{
  drawFrame();
}

void AreaSelect::mousePressEvent(QMouseEvent *e)
{
  kdDebug(4400) << "AreaSelect::mousePressEvent() handler called\n" << endl;
  
  if (isActive() && (e->button() == LeftButton)) {
    if (drawing) {
      kdWarning(4400) << "AreaSelect: Warning Left Button press received when pressed\n" << endl;
    }
    else {
      // erase old selection frame
      disableSelection();
      // mark new frame beginning
      startx= (e->pos()).x();
      starty= (e->pos()).y();
      lastx= startx;
      lasty= starty;
      drawing= TRUE;
    } 
  }
  if (!isActive()) {
    kdWarning(4400) << "AreaSelect: Warning event received when inactive (ignoring)\n" << endl;
  }
}

void AreaSelect::mouseMoveEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;

  if (isActive()) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    if ((lastx != x) || (lasty != y)) {
      if (drawing) {
	paint.begin(canvas->zoomedPixmap());
	paint.setPen(QPen(green, 0, DashLine));
	paint.setRasterOp(DEFAULT_RASTER_OP);

	// Erase old rectangle
	paint.drawRect(startx, starty, lastx-startx, lasty-starty);
	// Draw new rectangle
	paint.drawRect(startx, starty, x-startx, y-starty);

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

void AreaSelect::mouseReleaseEvent(QMouseEvent *e)
{
  int x,y;
  QPainter paint;
  QWMatrix m;

kdDebug(4400) << "AreaSelect::mouseReleaseEvent() handler called\n" << endl;

  if (isActive() && (e->button() == LeftButton)) {
    x= (e->pos()).x();
    y= (e->pos()).y();

    // Erase old line
    paint.begin(canvas->zoomedPixmap());
    paint.setPen(QPen(green, 0, DashLine));
    paint.setRasterOp(DEFAULT_RASTER_OP);
    paint.drawRect(startx, starty, lastx-startx, lasty-starty);

    paint.end();

    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    paint.begin(canvas->pixmap());
    //    paint.setPen(pen);
    paint.setWorldMatrix(m);
    paint.setRasterOp(CopyROP);
    canvas->setSelection(QRect(startx, starty, x-startx, y-starty));
    paint.end();
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);

    startTimer(TIMER_INTERVALL);
    showedSF= false;
  }
  else {
kdWarning(4400) << "Warning event received when inactive (ignoring)\n" << endl;
  }
}

QPixmap AreaSelect::pixmap() const
{
    return UserIcon("areaselect");
}

void 
AreaSelect::drawFrame()
{
  QPainter paint;
  paint.begin(canvas->zoomedPixmap());
  paint.setPen(QPen(green, 0, DashLine));
  paint.setRasterOp(DEFAULT_RASTER_OP);
  paint.drawRect(startx, starty, lastx-startx, lasty-starty);
  paint.end();
  canvas->repaint(0);
  showedSF = !showedSF;
}


#include "areaselect.moc"

#undef TIMER_INTERVALL /* for --enable-final */

