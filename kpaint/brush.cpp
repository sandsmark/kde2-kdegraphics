// $Id: brush.cpp 107633 2001-07-25 15:35:16Z rich $

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

#include "brush.h"

/* #################  Paste Tool ############################*/

PasteTool::PasteTool(const QString & toolname) : Tool(toolname)
{
  setSpecial( true );
  drawing= FALSE;
  showedSF= false;
  tooltip= i18n("Paste");
  props= 0;
}

/*
PasteTool::~PasteTool()
{ }
*/

void PasteTool::activating()
{
  kdDebug(4400) << "PasteTool::activating() hook called\n" << endl;
  
  cPixmap = kapp->clipboard()->pixmap();        
  noContent = cPixmap.isNull() ? true : false;
  if ( !noContent ) {
    lastx = -1;
    lasty = -1;
    canvas->setCursor( crossCursor );
    canvas->setMouseTracking( true );
  }
  else
    KMessageBox::sorry( 0, "The clipboard is empty!", "", false );
}

void PasteTool::deactivating()
{
  kdDebug(4400) << "PasteTool::deactivating() hook called\n" << endl;
  cPixmap.resize( 0, 0 );
  canvas->setMouseTracking( false );
}

void PasteTool::mousePressEvent(QMouseEvent *e)
{
  kdDebug(4400) << "PasteTool::mousePressEvent() handler called\n" << endl;
    
  if (isActive() && (e->button() == LeftButton) && !noContent) {
    if ( drawing )
      kdWarning(4400) << "PasteTool: Warning Left Button press received when pressed\n" << endl;
    else {
      QPainter paint;
      paint.begin(canvas->zoomedPixmap());

      if ( lastx != -1 ) {
	paint.setRasterOp( Qt::NotROP );
	paint.drawPixmap( lastx, lasty, cPixmap );
      }

      lastx = e->pos().x();
      lasty = e->pos().y();
      drawing= true;
        
      if ( !cPixmap.isNull() ) {
	paint.setRasterOp( Qt::CopyROP );
	paint.drawPixmap( lastx, lasty, cPixmap );
      }    
      paint.end();
      canvas->repaint(0);
    }
  }
  if ( !isActive() ) {
    kdWarning(4400) << "PasteTool: Warning event received when inactive (ignoring)\n" << endl;
  }
}



void PasteTool::mouseMoveEvent(QMouseEvent *e)
{
  if ( isActive() && !noContent ) {
    int x = e->pos().x();
    int y = e->pos().y();
    
    if ((lastx != x) || (lasty != y)) {
      QPainter paint;
      paint.begin( canvas->zoomedPixmap() );

      if ( drawing ) {
	canvas->updateZoomed(); // TODO: Only update the part that was obscured
	paint.drawPixmap( x, y, cPixmap );
      }
      else {
	if ( lastx != -1 ) {
	  paint.setRasterOp( Qt::NotROP );
	  paint.drawPixmap( lastx, lasty, cPixmap );
	}
	paint.setRasterOp( Qt::NotROP );
	paint.drawPixmap( x, y, cPixmap );                     
      }    

      paint.end();
      lastx = x;
      lasty = y;
      canvas->repaint(0);
    }
  }
  else {
    kdWarning(4400) << "Warning event received when inactive (ignoring)\n" << endl;
  }
}


void PasteTool::mouseReleaseEvent(QMouseEvent *e)
{
  kdDebug(4400) << "PasteTool::mouseReleaseEvent() handler called\n" << endl;

  if (isActive() && (e->button() == LeftButton) && !noContent) 
  {
    int x= e->pos().x();
    int y= e->pos().y();

    QWMatrix m;
    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));

    QPainter paint;
    paint.begin( canvas->pixmap() );
    paint.setWorldMatrix(m);
    paint.setRasterOp(CopyROP);

    if( !cPixmap.isNull() )
      paint.drawPixmap( x, y, cPixmap );
    paint.end();
    lastx = -1;
    lasty = -1;
    drawing= FALSE;
    canvas->updateZoomed();
    canvas->repaint(0);

    showedSF= false;
  }
  else 
  {
    kdWarning(4400) << "Warning event received when inactive (ignoring)\n" << endl;
  }
}

QPixmap PasteTool::pixmap() const
{
    return UserIcon("brush");
}

#include "brush.moc"



