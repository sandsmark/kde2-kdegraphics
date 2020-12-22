// $Id: canvas.cpp 141746 2002-03-08 12:38:27Z mlaurent $

#include <kdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include <qpainter.h>
#include <qimage.h>
#include <qwmatrix.h>
#include <qclipboard.h>

#include <klocale.h>
#include <kapp.h>

#include <math.h>

#include "canvas.h"
#include "tool.h"

//#define CUT_FILL_RGB 0xFFFFFF //white

int Canvas::inst = 0;

Canvas::Canvas(int width, int height, QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  currentTool= 0;
  s= INACTIVE;
  matrix= new QWMatrix;
  zoomed= 0;

  // Create pixmap
  pix= new QPixmap(width, height);
  if (!pix) {
    kdDebug(4400) << "Canvas::Canvas(): Cannot create pixmap\n" << endl;
    exit(1);
  }

  pix->fill(QColor("white"));

  setZoom(100);

  // Set keyboard focus policy
  setFocusPolicy(QWidget::StrongFocus);
  emit sizeChanged();

  Canvas::inst++;
  
}

Canvas::Canvas(const QString & filename, QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  currentTool= 0;
  s= INACTIVE;
  zoomed= 0;
  matrix= new QWMatrix;

  // Create pixmap
  pix= new QPixmap(filename);
  if (!pix) 
  {
    kdDebug(4400) << "Canvas::Canvas(): Cannot create pixmap\n" << endl;
    exit(1);
  }

  resize(pix->width(), pix->height());

  setZoom(100);

  emit sizeChanged();

  // Set keyboard focus policy
  setFocusPolicy(QWidget::StrongFocus);
 
  Canvas::inst++;
  
}

Canvas::~Canvas()
{
  delete pix;
  delete zoomed;
  delete matrix;
}   

bool Canvas::isModified()
{
    return modified_;
}

void Canvas::clearModified()
{
    modified_= false;
}

void Canvas::markModified()
{
    modified_= true;
    kdDebug(4400) << "Canvas: emitting modified()\n" << endl;
    emit modified();
}

void Canvas::setSelection(const QRect &rect)
{
    selection_= rect.normalize();
    haveSelection_= true;
}

const QRect &Canvas::selection() const
{
    return selection_;
}

void Canvas::clearSelection()
{
    haveSelection_= false;
    selection_= QRect(0,0,0,0);
}


// -------- CUT / COPY / PASTE ------------

void Canvas::cut()
{
  copy();

  int w  = haveSelection_ ? selection_.right() - selection_.left() : pix->width();
  int h  = haveSelection_ ? selection_.bottom() - selection_.top() : pix->height();
  QPixmap p( w, h );
  p.fill(QColor("white"));
  
  int x = haveSelection_ ? selection_.left() : 0;
  int y  = haveSelection_ ? selection_.top() : 0;
  bitBlt( pix, x, y, &p, 0, 0, w, h, CopyROP);

  clearSelection();
  markModified();
  updateZoomed();
}


void Canvas::copy()
{
  int x = haveSelection_ ? selection_.left() : 0;
  int y  = haveSelection_ ? selection_.top() : 0;
  int w  = haveSelection_ ? selection_.right() - selection_.left() : pix->width();
  int h  = haveSelection_ ? selection_.bottom() - selection_.top() : pix->height();
  kdDebug(4400) << "copying: (" << x << ", " << y << ") - (" << w << ", " << h << ")";

  QPixmap clipboardPix( w, h );
  bitBlt( &clipboardPix, 0, 0, pix, x, y, w, h, CopyROP);
  kapp->clipboard()->setPixmap( clipboardPix ); 
}

void Canvas::paste( int x, int y )
{
    QClipboard *cb = kapp->clipboard();
    QPixmap clipPix = cb->pixmap();
    if ( !clipPix.isNull() ) {
       bitBlt( pix, x, y, &clipPix, 0, 0, clipPix.width(), clipPix.height(), CopyROP );
       markModified();
       updateZoomed();
    }
}


// ---------- ZOOM ------------------------

void Canvas::setZoom(int z)
{
  QPainter p;
  int w, h;

  zoomFactor= z;
  matrix->reset();
  matrix->scale((float) z/100, (float) z/100);

  if (zoomed != 0)
    delete zoomed;

  w= (int) (pix->width()* ((float) zoomFactor/100));
  h= (int) (pix->height()*((float) zoomFactor/100));

  zoomed= new QPixmap(w, h);
  zoomed->fill(QColor("white"));

  p.begin(zoomed);
  p.setWorldMatrix(*matrix);
  p.drawPixmap(0,0,*pix);
  p.end();

  if ((w != width()) || (h != height())) {
    resize(w,h);
    emit sizeChanged();
  }
  repaint(0);
}

void Canvas::updateZoomed()
{
  QPainter p;
/*  int w, h; */

  zoomed->fill(QColor("white"));

  p.begin(zoomed);
  p.setWorldMatrix(*matrix);
  p.drawPixmap(0,0,*pix);
  p.end();

  repaint( false );
}

/*
    The offscreen clipboard pixmap must also be adjusted to
    zoom factor for pasting to a zoomed canvas pixmap
*/

void Canvas::updateZoomedClipboard()
{
  if (!clipboardPix.isNull())
  {      
    QPainter p;

    int clipWid = clipboardPix.width();
    int clipHgt = clipboardPix.height();
    
    int newClipWid = (int) (clipWid * ((float) zoomFactor/100));
    int newClipHgt = (int) (clipHgt * ((float) zoomFactor/100));
        
    /* make copy of clipboard data to temporary pixmap */
    QPixmap *zc = new QPixmap(clipWid, clipHgt);    
    zc->fill(QColor("white"));
    p.begin(zc);
    p.drawPixmap(0,0, clipboardPix);
    p.end();

    /* adjust clipboard size to zoom factor */
    clipboardPix.resize(QSize(newClipWid, newClipHgt));
    clipboardPix.fill(QColor("white"));

    /* copy to adjusted clipboard using matrix */
    p.begin(&clipboardPix);
    p.setWorldMatrix(*matrix);
    p.drawPixmap(0,0,*zc);
    p.end();
    
    /* clean up */
    delete zc;
  }
}


int Canvas::zoom()
{
  return zoomFactor;
}

void Canvas::activate(Tool *t)
{
  assert(!isActive());
  currentTool= t;
  s= ACTIVE;
}

void Canvas::deactivate()
{
  assert(isActive());
  s= INACTIVE;
  currentTool= 0;
}

QPixmap *Canvas::pixmap()
{
  return pix;
}

QPixmap *Canvas::zoomedPixmap()
{
  return zoomed;
}

void Canvas::setPixmap(QPixmap *px)
{
    QPainter p;
    int w, h;

    *pix= *px;
    emit pixmapChanged(pix);

    delete zoomed;

    w = (int) (px->width()* ((float) zoomFactor/100));
    h = (int) (px->height()*((float) zoomFactor/100));

    zoomed = new QPixmap(w, h);

    p.begin(zoomed);
    p.setWorldMatrix(*matrix);
    p.drawPixmap(0,0,*pix);
    p.end();

    if ((w != width()) || (h != height())) 
    {
        resize(w,h);
        emit sizeChanged();
    }
    repaint(0);
}

void Canvas::setDepth(int d)
{
  QImage i;
  QPixmap *px;

  assert((d == 1) || (d == 4) || (d == 8) || 
	 (d == 15) || (d == 16) ||
	 (d == 24) || (d == 32));

  if (d != pix->depth()) {
    i= pix->convertToImage();
    i.convertDepth(d);
    px= new QPixmap(pix->width(), pix->height(), d);
    *px= i;
    setPixmap(px);
    markModified();
    delete px;
  }
}

void Canvas::resizeImage(int w, int h)
{
  QWMatrix matrix;
  QPainter p;

  if ((w != pix->width()) || (h != pix->height())) {
    QPixmap *newpix= new QPixmap(w, h);
    matrix.scale((float) w/pix->width(), (float) h/pix->height());
    p.begin(newpix);
    p.setWorldMatrix(matrix);
    p.drawPixmap(0,0,*pix);
    p.end();

    delete pix;
    pix= newpix;
    setZoom(zoom());
    emit pixmapChanged(pix);
    markModified();
  }
  repaint(0);
}

/* 
    paint Event is the ONLY place where the canvas is
    updated, normally, by copying the offscreen zoomed pixmap
    to the canvas    
*/

void Canvas::paintEvent(QPaintEvent *)
{
  bitBlt(this, 0, 0, zoomed);
}

void Canvas::mousePressEvent(QMouseEvent *e)
{
kdDebug(4400) << "Canvas::mousePressEvent() redirector called\n" << endl;
  if (isActive())
    currentTool->mousePressEvent(e);
} 

void Canvas::mouseMoveEvent(QMouseEvent *e)
{
  if (isActive())
    currentTool->mouseMoveEvent(e);
}

void Canvas::mouseReleaseEvent(QMouseEvent *e)
{
  kdDebug(4400) << "Canvas::mouseReleaseEvent() redirector called\n" << endl;
  if ( isActive() )
    currentTool->mouseReleaseEvent(e);
}

bool Canvas::isActive()
{
  if (s == ACTIVE)
    return true;
  else
    return false;
}

bool Canvas::load(const QString & filename, const char *format)
{
  bool s;
  QPixmap p;
  QPixmap q; // Fix UMR when reading transparent pixels (they hold junk)

  if (!format) 
  { 
    s = p.load(filename);
  } 
  else 
  {
    s = p.load(filename, format);
  }

  if (s) 
  {
    q.resize(p.size());
    q.fill(QColor("white"));
    bitBlt(&q, 0, 0, &p);
    setPixmap(&q);
  }

  repaint(0);

  return s;
}

bool Canvas::save(const QString & filename, const char *format)
{
  bool s;

kdDebug(4400) << "Canvas::save() file= " << filename << ", format= " << format << "\n" << endl;

  s= pix->save(filename, format);

kdDebug(4400) << "Canvas::save() returning " << s << "\n" << endl;

  return s;
}

void Canvas::keyPressEvent(QKeyEvent *e)
{
  //kdDebug(4400) << "Canvas::keyPressEvent() redirector called\n" << endl;
  if (isActive())
    currentTool->keyPressEvent(e);
}


void Canvas::keyReleaseEvent(QKeyEvent *e)
{
  //kdDebug(4400) << "Canvas::keyReleaseEvent() redirector called\n" << endl;
  if (isActive())
    currentTool->keyReleaseEvent(e);
}


#include "canvas.moc"
