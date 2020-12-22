/*
    KDE Icon Editor - a small graphics drawing program for the KDE.
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <stdlib.h>

#include <kpixmap.h>
#include <kruler.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "kicongrid.h"
#include "main.h"
#ifndef PICS_INCLUDED
#include "pics/logo.xpm"
#define PICS_INCLUDED
#endif

KGridView::KGridView(QImage *image, QWidget *parent, const char *name) 
: QFrame(parent, name)
{
  pprops = 0L;
  _corner = 0L;
  _hruler = _vruler = 0L;
  _grid = 0L;

  acceptdrop = false;

  pprops = props(this);

  viewport = new QScrollView(this);
  CHECK_PTR(viewport);

  _grid = new KIconEditGrid(image, viewport->viewport());
  CHECK_PTR(_grid);
  viewport->addChild(_grid);

  if(pprops->backgroundmode == FixedPixmap)
  {
    QPixmap pix(pprops->backgroundpixmap);
    if(pix.isNull())
    {
      QPixmap pmlogo((const char **)logo);
      pix = pmlogo;
    }
    viewport->viewport()->setBackgroundPixmap(pix);
    _grid->setBackgroundPixmap(pix);
  }
  else
  {
    viewport->viewport()->setBackgroundColor(pprops->backgroundcolor);
  }

  _corner = new QFrame(this);
  _corner->setFrameStyle(QFrame::WinPanel | QFrame::Raised);

  _hruler = new KRuler(Qt::Horizontal, this);
  _hruler->setEndLabel(i18n("width"));
  _hruler->setOffset( -2 );
  _hruler->setRange(0, 1000);

  _vruler = new KRuler(Qt::Vertical, this);
  _vruler->setEndLabel(i18n("height"));
  _vruler->setOffset( -2 );
  _vruler->setRange(0, 1000);

  connect(_grid, SIGNAL(scalingchanged(int, bool)), SLOT(scalingChange(int, bool)));
  connect(_grid, SIGNAL(sizechanged(int, int)), SLOT(sizeChange(int, int)));
  connect(_grid, SIGNAL(needPainting()), SLOT(paintGrid()));
  connect( _grid, SIGNAL(xposchanged(int)), _hruler, SLOT(slotNewValue(int)) );
  connect( _grid, SIGNAL(yposchanged(int)), _vruler, SLOT(slotNewValue(int)) );
  connect(viewport, SIGNAL(contentsMoving(int, int)), SLOT(moving(int, int)));

  setSizes();
  QResizeEvent e(size(), size());
  resizeEvent(&e);
}

void KGridView::paintGrid()
{
  _grid->repaint(viewRect(), false);
}

void KGridView::setSizes()
{
  if(pprops->showrulers)
  {
    _hruler->setLittleMarkDistance(_grid->scaling());
    _vruler->setLittleMarkDistance(_grid->scaling());

    _hruler->setMediumMarkDistance(5);
    _vruler->setMediumMarkDistance(5);

    _hruler->setBigMarkDistance(10);
    _vruler->setBigMarkDistance(10);

    _hruler->setShowTinyMarks(true);
    _hruler->setShowLittleMarks(false);
    _hruler->setShowMediumMarks(true);
    _hruler->setShowBigMarks(true);
    _hruler->setShowEndMarks(true);

    _vruler->setShowTinyMarks(true);
    _vruler->setShowLittleMarks(false);
    _vruler->setShowMediumMarks(true);
    _vruler->setShowBigMarks(true);
    _vruler->setShowEndMarks(true);

    _hruler->setValuePerLittleMark(_grid->scaling());
    _vruler->setValuePerLittleMark(_grid->scaling());
    _hruler->setValuePerMediumMark(_grid->scaling()*5);
    _vruler->setValuePerMediumMark(_grid->scaling()*5);

    _hruler->setPixelPerMark(_grid->scaling());
    _vruler->setPixelPerMark(_grid->scaling());

    _hruler->setMaxValue(_grid->width()+20);
    _vruler->setMaxValue(_grid->height()+20);

    _hruler->show();
    _vruler->show();

    _corner->show();
    //resize(_grid->width()+_vruler->width(), _grid->height()+_hruler->height());
  }
  else
  {
    _hruler->hide();
    _vruler->hide();
    _corner->hide();
    //resize(_grid->size());
  }
}

void KGridView::sizeChange(int, int)
{
    setSizes();
}

void KGridView::moving(int x, int y)
{
    //kdDebug() << "Moving: " << x << " x " << y << endl;
    _hruler->setOffset(abs(x));
    _vruler->setOffset(abs(y));
}

void KGridView::scalingChange(int, bool)
{
    setSizes();
}

void KGridView::setShowRulers(bool mode)
{
    pprops->showrulers = mode;
    setSizes();
    QResizeEvent e(size(), size());
    resizeEvent(&e);
}

void KGridView::setAcceptDrop(bool a)
{
    if(a == acceptdrop) return;
    acceptdrop = a;
    paintDropSite();
}

const QRect KGridView::viewRect()
{
    int x, y, cx, cy;
    if(viewport->horizontalScrollBar()->isVisible())
    {
        x = viewport->contentsX();
        cx = viewport->viewport()->width();
    }
    else
    {
        x = 0;
        cx = viewport->contentsWidth();
    }

    if(viewport->verticalScrollBar()->isVisible())
    {
        y = viewport->contentsY();
        cy = viewport->viewport()->height();
    }
    else
    {
        y = 0;
        cy = viewport->contentsHeight();
    }

    return QRect(x, y, cx, cy);
}

void KGridView::paintDropSite()
{
    QPainter p;
    p.begin( _grid );
    p.setRasterOp (NotROP);
    p.drawRect(viewRect());
    p.end();
}

void KGridView::paintEvent(QPaintEvent *)
{
  if(acceptdrop)
    paintDropSite();
}


void KGridView::resizeEvent(QResizeEvent*)
{
  kdDebug() << "KGridView::resizeEvent" << endl;
  
  setSizes();
  
  if(pprops->showrulers)
  {
    _hruler->setGeometry(_vruler->width(), 0, width(), _hruler->height());
    _vruler->setGeometry(0, _hruler->height(), _vruler->width(), height());

    _corner->setGeometry(0, 0, _vruler->width(), _hruler->height());
    viewport->setGeometry(_corner->width(), _corner->height(),
                   width()-_corner->width(), height()-_corner->height());
  }
  else
    viewport->setGeometry(0, 0, width(), height());
}


/*
    replace a pixel with alpha channel value less than 0xff, 
    with any transparency at all, with the default background color
*/
const QImage *fixTransparence(QImage *image)
{
    *image = image->convertDepth(32);
    image->setAlphaBuffer(true);
    for(int y = 0; y < image->height(); y++)
    {
        uint *l = (uint*)image->scanLine(y);
        for(int x = 0; x < image->width(); x++, l++)
        {
            if(*l < OPAQUE_MASK || *l == (TRANSPARENT|OPAQUE_MASK))
            {
                *l = TRANSPARENT;
            }
        }
    }
    return image;
}

/*
    if a pixel is default background color, add alpha value
    ( QColor ARGB & 00ffffff) to make it transparent in saved image
*/
const QImage *saveTransparence(QImage *image)
{
    *image = image->convertDepth(32);
    image->setAlphaBuffer(true);
    for(int y = 0; y < image->height(); y++)
    {
        uint *l = (uint*)image->scanLine(y);
        for(int x = 0; x < image->width(); x++, l++)
        {
            if(*l == TRANSPARENT)
            {
                *l &= ~(TRANSPARENT);
            }
        }
    }
    return image;
}


KIconEditGrid::KIconEditGrid(QImage *image, QWidget *parent, const char *name)
 : KColorGrid(parent, name, 1)
{
    img = image;
    selected = 0;

    // the 42 normal kde colors - there can be an additional
    // 18 custom colors in the custom colors palette
    for(uint i = 0; i < 42; i++)
        iconcolors.append(iconpalette[i]);

    setupImageHandlers();
    //kdDebug() << "Formats: " << formats->count() << endl;
    btndown = isselecting = ispasting = modified = false;

    img->create(32, 32, 32);
    img->fill(TRANSPARENT);
    
    currentcolor = qRgb(0,0,0)|OPAQUE_MASK;

    setMouseTracking(true);

    setNumRows(32);
    setNumCols(32);
    fill(TRANSPARENT);

    connect( kapp->clipboard(), SIGNAL(dataChanged()), SLOT(checkClipboard()));
    createCursors();
}

KIconEditGrid::~KIconEditGrid()
{
    kdDebug() << "KIconEditGrid - destructor: done" << endl;
}

void KIconEditGrid::paintEvent(QPaintEvent *e)
{
    //kdDebug() << "KIconGrid::paintEvent" << endl;
    KColorGrid::paintEvent(e);

    if(hasGrid()&& !(cellSize()==1))
    {
        int x = e->rect().x() - ((e->rect().x() % cellSize()) + cellSize());
        if(x < 0) x = 0;
        int y = e->rect().y() - ((e->rect().y() % cellSize()) + cellSize());
        if(y < 0) y = 0;
        int cx = e->rect().right() + cellSize();
        int cy = e->rect().bottom() + cellSize();

        QPainter p;
        p.begin( this );
    
        // draw grid lines
        for(int i = x; i < cx; i += cellSize())
            p.drawLine(i, y, i, cy);

        for(int i = y; i < cy; i += cellSize())
            p.drawLine(x, i, cx, i);

        p.end();
    }
    //kdDebug() << "KIconGrid::paintEvent - done" << endl;
}


void KIconEditGrid::paintCell( QPainter *painter, int row, int col )
{
  int sel = row * numCols() + col;
  int s = cellSize();
  uint c = colorAt(sel);

  if(s == 1)
  {
    if((ispasting || isselecting) && isMarked(col, row))
    {
      painter->drawPoint(0, 0);
      return;
    }
    else
    {
      painter->setPen(c);
      painter->drawPoint(0, 0);
    }
  }
  else
  {
    painter->fillRect(0, 0, s, s, (QBrush)c); 
    if((ispasting || isselecting) && isMarked(col, row))
    {
      painter->drawWinFocusRect( 1, 1, s-1, s-1);
      return;
    }
  }

  switch( tool )
  {
    case Find:
    case Spray:
    case Eraser:
    case Freehand:
      if ( sel == selected)
      {
        if(cellSize() > 1)
          painter->drawWinFocusRect( 1, 1, s-1, s-1 );
        else
          painter->drawPoint(0, 0);
      }
      break;
    
    case FilledRect:
    case Rect:
    case Ellipse:
    case Circle:
    case FilledEllipse:
    case FilledCircle:
    case Line:
      if(btndown && isMarked(col, row))
      {
        if(cellSize() > 1)
          painter->drawWinFocusRect( 1, 1, s-1, s-1);
        else
          painter->drawPoint(0, 0);
      }
      break;
    
    default:
      break;
  }
  //kdDebug() << "KIconEditGrid::paintCell() - done" << endl;
}

void KIconEditGrid::mousePressEvent( QMouseEvent *e )
{
    if(!e || (e->button() != LeftButton))
        return;

    int row = findRow( e->pos().y() );
    int col = findCol( e->pos().x() );
    //int cell = row * numCols() + col;

    if(!img->valid(col, row))
        return;

    btndown = true;
    start.setX(col);
    start.setY(row);

    if(ispasting)
    {
        ispasting = false;
        editPaste(true);
    }

    if(isselecting)
    {
        QPointArray a(pntarray.copy());
        pntarray.resize(0);
        drawPointArray(a, Mark);
        emit selecteddata(false);
    }

    switch( tool )
    {
        case SelectRect:
        case SelectCircle:
            isselecting = true;
            break;
        default:
            break;
    }
}

void KIconEditGrid::mouseMoveEvent( QMouseEvent *e )
{
    if(!e) return;

    int row = findRow( e->pos().y() );
    int col = findCol( e->pos().x() );
    int cell = row * numCols() + col;

    QPoint tmpp(col, row);
    if(tmpp == end) return;

    if(img->valid(col, row))
    {
        //kdDebug() << col << " X " << row << endl;
        emit poschanged(col, row);
        // for the rulers
        emit xposchanged((col*scaling())+scaling()/2); 
        emit yposchanged((row*scaling())+scaling()/2);
    }

    // need to use intersection of rectangles to allow pasting
    // only that part of clip image which intersects -jwc-
    if(ispasting && !btndown && img->valid(col, row))
    {
        if( (col + cbsize.width()) > (numCols()-1) )
            insrect.setX(numCols()-insrect.width());
        else
            insrect.setX(col);
        if( (row + cbsize.height()) > (numRows()-1) )
            insrect.setY(numRows()-insrect.height());
        else
            insrect.setY(row);
      
        insrect.setSize(cbsize);
        start = insrect.topLeft();
        end = insrect.bottomRight();
        drawRect(false);
        return;
    }

    if(!img->valid(col, row) || !btndown)
        return;

    end.setX(col);
    end.setY(row);

    if(isselecting)
    {
        if(tool == SelectRect)
            drawRect(false);
        else
            drawEllipse(false);
        return;
    }
    
    bool erase=false;
    switch( tool )
    {
        case Eraser:
            erase=true;

        case Freehand:
        {
            if(erase)
                setColor( cell, TRANSPARENT );
            else
                setColor( cell, currentcolor );

            if ( selected != cell )
            {
                modified = true;
                int prevSel = selected;
                selected = cell;
                repaint((prevSel%numCols())*cellsize,(prevSel/numCols())*cellsize, cellsize, cellsize, false);
                repaint(col*cellsize,row*cellsize, cellsize, cellsize, false);
                *((uint*)img->scanLine(row) + col) = (colorAt(cell));
            }
            break;
        }
        case Find:
        {
            iconcolors.closestMatch(colorAt(cell));
            if ( selected != cell )
            {
                int prevSel = selected;
                selected = cell;
                repaint((prevSel%numCols())*cellsize,(prevSel/numCols())*cellsize, cellsize, cellsize, false);
                repaint(col*cellsize,row*cellsize, cellsize, cellsize, false);
            }
            break;
        }
        case Ellipse:
        case Circle:
        case FilledEllipse:
        case FilledCircle:
        {
            drawEllipse(false);
            break;
        }
        case FilledRect:
        case Rect:
        {
            drawRect(false);
            break;
        }
        case Line:
        {
            drawLine(false, false);
            break;
        }
        case Spray:
        {
            drawSpray(QPoint(col, row));
            modified = true;
            break;
        }
        default:
            break;
    }

    p = *img;
    emit changed(QPixmap(p));
}

void KIconEditGrid::mouseReleaseEvent( QMouseEvent *e )
{
  if(!e || (e->button() != LeftButton))
    return;

  int row = findRow( e->pos().y() );
  int col = findCol( e->pos().x() );
  btndown = false;
  end.setX(col);
  end.setY(row);
  int cell = row * numCols() + col;
  bool erase=false;
  switch( tool )
  {
    case Eraser:
        erase=true;
      //currentcolor = TRANSPARENT;
    case Freehand:
    {
      if(!img->valid(col, row))
        return;
      if(erase)
        setColor( cell, TRANSPARENT );
      else
        setColor( cell, currentcolor );
      //if ( selected != cell )
      //{
        //modified = true;
        int prevSel = selected;
        selected = cell;
        repaint((prevSel%numCols())*cellsize,(prevSel/numCols())*cellsize, cellsize, cellsize, false);
        repaint(col*cellsize,row*cellsize, cellsize, cellsize, false);
        //updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
        //updateCell( row, col, FALSE );
        *((uint*)img->scanLine(row) + col) = colorAt(cell);
        p = *img;
      //}
      break;
    }
    case Ellipse:
    case Circle:
    case FilledEllipse:
    case FilledCircle:
    {
      drawEllipse(true);
      break;
    }
    case FilledRect:
    case Rect:
    {
      drawRect(true);
      break;
    }
    case Line:
    {
      drawLine(true, false);
      break;
    }
    case Spray:
    {
      drawSpray(QPoint(col, row));
      break;
    }
    case FloodFill:
    {
      QApplication::setOverrideCursor(waitCursor);
      drawFlood(col, row, colorAt(cell));
      QApplication::restoreOverrideCursor();
      updateColors();
      emit needPainting();
      p = *img;
      break;
    }
    case Find:
    {
      currentcolor = colorAt(cell);
      if ( selected != cell )
      {
        int prevSel = selected;
        selected = cell;
        repaint((prevSel%numCols())*cellsize,(prevSel/numCols())*cellsize, cellsize, cellsize, false);
        repaint(col*cellsize,row*cellsize, cellsize, cellsize, false);
        //updateCell( prevSel/numCols(), prevSel%numCols(), FALSE );
        //updateCell( row, col, FALSE );
      }

      break;
    }
    default:
      break;
  }
  emit changed(QPixmap(p));
  //emit colorschanged(numColors(), data());
}

//void KIconEditGrid::setColorSelection( const QColor &color )
void KIconEditGrid::setColorSelection( uint c )
{
  currentcolor = c;
}

void KIconEditGrid::loadBlank( int w, int h )
{
  img->create(w, h, 32);
  img->fill(TRANSPARENT);
  setNumRows(h);
  setNumCols(w);
  fill(TRANSPARENT);
  emit sizechanged(numCols(), numRows());
  emit colorschanged(numColors(), data());
}



void KIconEditGrid::load( QImage *image)
{
  kdDebug() << "KIconEditGrid::load" << endl;

    setUpdatesEnabled(false);

    if(image != 0L)
    {
        *img = *fixTransparence(image);
    }
    else
    {
        QString msg = i18n("There was an error loading a blank image.\n");
        KMessageBox::error(this, msg);
        return;
    }

    setNumRows(img->height());
    setNumCols(img->width());

    for(int y = 0; y < numRows(); y++)
    {
        uint *l = (uint*)img->scanLine(y);
        for(int x = 0; x < numCols(); x++, l++)
        {
            setColor((y*numCols())+x, *l, false);
        }
        //kdDebug() << "Row: " << y << endl;
        kapp->processEvents(200);
    }
  
    updateColors();
    emit sizechanged(numCols(), numRows());
    emit colorschanged(numColors(), data());
    emit changed(pixmap());
    setUpdatesEnabled(true);
    emit needPainting();
    //repaint(viewRect(), false);
}

const QPixmap &KIconEditGrid::pixmap()
{
    if(!img->isNull())
        p = *img;
    //p.convertFromImage(*img, 0);
    return(p);
}

void KIconEditGrid::getImage(QImage *image)
{
    kdDebug() << "KIconEditGrid::getImage" << endl;
    *image = *img;
}

bool KIconEditGrid::zoomTo(int scale)
{
    QApplication::setOverrideCursor(waitCursor);
    emit scalingchanged(cellSize(), false);
    setUpdatesEnabled(false);
    setCellSize( scale );
    setUpdatesEnabled(true);
    emit needPainting();
    QApplication::restoreOverrideCursor();
    emit scalingchanged(cellSize(), true);
  
    if(scale == 1)
        return false;
    return true;
}

bool KIconEditGrid::zoom(Direction d)
{
    int f = (d == In) ? (cellSize()+1) : (cellSize()-1);
    QApplication::setOverrideCursor(waitCursor);
    emit scalingchanged(cellSize(), false);
    setUpdatesEnabled(false);
    setCellSize( f );
    setUpdatesEnabled(true);
    //emit needPainting();
    QApplication::restoreOverrideCursor();

    emit scalingchanged(cellSize(), true);
    if(d == Out && cellSize() <= 1)
        return false;
    return true;
}

void KIconEditGrid::checkClipboard()
{
  bool ok = false;
  QImage tmp = clipboardImage(ok);
  if(ok)
    emit clipboarddata(true);
  else
    emit clipboarddata(false);
}

QImage KIconEditGrid::clipboardImage(bool &ok)
{
  QImage image = kapp->clipboard()->image();
  ok = !image.isNull();
  if ( ok ) 
  {
      image = image.convertDepth(32);
      image.setAlphaBuffer(true);
  }
  return image;
}


void KIconEditGrid::editSelectAll()
{
    start.setX(0);
    start.setY(0);
    end.setX(numCols());
    end.setY(numRows());
    isselecting = true;
    drawRect(false);
    emit newmessage(i18n("All selected"));
}

void KIconEditGrid::editClear()
{
    img->fill(TRANSPARENT);
    fill(TRANSPARENT);
    update();
    modified = true;
    p = *img;
    emit changed(p);
    emit newmessage(i18n("Cleared"));
}

QImage KIconEditGrid::getSelection(bool cut)
{
    const QRect rect = pntarray.boundingRect();
    int nx = 0, ny = 0, nw = 0, nh = 0;
    rect.rect(&nx, &ny, &nw, &nh);

    QImage tmp(nw, nh, 32);
    tmp.fill(TRANSPARENT);

    int s = pntarray.size(); 
    
    for(int i = 0; i < s; i++)
    {
        int x = pntarray[i].x();
        int y = pntarray[i].y();
        if(img->valid(x, y) && rect.contains(QPoint(x, y)))
        {
            *((uint*)tmp.scanLine(y-ny) + (x-nx)) = *((uint*)img->scanLine(y) + x);
            if(cut)
            {
                *((uint*)img->scanLine(y) + x) = TRANSPARENT;
                setColor( (y*numCols()) + x, TRANSPARENT, false );
            }
        }
    }

    QPointArray a(pntarray.copy());
    pntarray.resize(0);
    drawPointArray(a, Mark);
    emit selecteddata(false);
    if(cut)
    {
        updateColors();
        repaint(rect.x()*cellSize(), rect.y()*cellSize(),
            rect.width()*cellSize(), rect.height()*cellSize(), false);
        p = *img;
        emit changed(p);
        emit colorschanged(numColors(), data());
        emit newmessage(i18n("Selected area cut"));
    }
    else
        emit newmessage(i18n("Selected area copied"));

    return tmp;
}

void KIconEditGrid::editCopy(bool cut)
{
    kapp->clipboard()->setImage(getSelection(cut));
    isselecting = false;
}


void KIconEditGrid::editPaste(bool paste)
{
    bool ok = false;
    QImage tmp = clipboardImage(ok);
    fixTransparence(&tmp);

    Properties *pprops = props(this);

    if(ok)
    {
        if( (tmp.size().width() > img->size().width()) 
        || (tmp.size().height() > img->size().height()) )
        {
            if(KMessageBox::warningYesNo(this,
                i18n("The clipboard image is larger than the current image!\nPaste as new image?")) == 0)
            {
                editPasteAsNew();
            }
            return;
        }
        else if(!paste)
        {
            ispasting = true;
            cbsize = tmp.size();
            return;
            // emit newmessage(i18n("Pasting"));
        }
        else
        {
            //kdDebug() << "KIconEditGrid: Pasting at: " << insrect.x() << " x " << insrect.y() << endl;
            QApplication::setOverrideCursor(waitCursor);

            for(int y = insrect.y(), ny = 0; y < numRows(), ny < insrect.height(); y++, ny++)
            {
                uint *l = ((uint*)img->scanLine(y)+insrect.x());
                uint *cl = (uint*)tmp.scanLine(ny);
                for(int x = insrect.x(), nx = 0; x < numCols(), nx < insrect.width(); x++, nx++, l++, cl++)
                {
                    if(*cl != TRANSPARENT || pprops->pastetransparent)
                    {
                        *l = *cl;
                        setColor((y*numCols())+x, (uint)*cl, false);
                    }
                }
            }
            updateColors();
            repaint(insrect.x()*cellSize(), insrect.y()*cellSize(),
                insrect.width()*cellSize(), insrect.height()*cellSize(), false);

            QApplication::restoreOverrideCursor();

            modified = true;
            p = *img;
            emit changed(QPixmap(p));
            emit sizechanged(numCols(), numRows());
            emit colorschanged(numColors(), data());
            emit newmessage(i18n("Done pasting"));
        }
    }
    else
    {
        QString msg = i18n("Invalid pixmap data in clipboard!\n");
        KMessageBox::sorry(this, msg);
    }
}


void KIconEditGrid::editPasteAsNew()
{
    bool ok = false;
    QImage tmp = clipboardImage(ok);

    if(ok)
    {
        *img = tmp;
        load(img);
        modified = true;
        //repaint(viewRect(), false);

        p = *img;
        emit changed(QPixmap(p));
        emit sizechanged(numCols(), numRows());
        emit colorschanged(numColors(), data());
        emit newmessage(i18n("Done pasting"));
    }
    else
    {
        QString msg = i18n("Invalid pixmap data in clipboard!\n");
        KMessageBox::error(this, msg);
    }
}


void KIconEditGrid::editResize()
{
    kdDebug() << "KIconGrid::editResize" << endl;
    KResizeDialog *rs = new KResizeDialog(this, 0, QSize(numCols(), numRows()));
    if(rs->exec())
    {
        const QSize s = rs->getSize();
        *img = img->smoothScale(s.width(), s.height());
        load(img);

        modified = true;
    }
    delete rs;
}


void KIconEditGrid::setSize(const QSize s)
{
    kdDebug() << "::setSize: " << s.width() << " x " << s.height() << endl;
  
    img->create(s.width(), s.height(), 32);
    img->fill(TRANSPARENT);
    load(img);
}


void KIconEditGrid::createCursors()
{
  QBitmap mask(22, 22);
  QPixmap pix;

  cursor_normal = QCursor(arrowCursor);

  pix = BarIcon("paintbrush-cursor");
  if(pix.isNull())
  {
    cursor_paint = cursor_normal;
    kdDebug() << "KIconEditGrid: Error loading paintbrush.xpm" << endl;
  }
  else
  {
    mask = pix.createHeuristicMask();
    pix.setMask(mask);
    cursor_paint = QCursor(pix, 6, 18);
  }

  pix = BarIcon("flood-cursor");
  if(pix.isNull())
  {
    cursor_flood = cursor_normal;
    kdDebug() << "KIconEditGrid: Error loading flood-cursor.xpm" << endl;
  }
  else
  {
    mask = pix.createHeuristicMask();
    pix.setMask(mask);
    cursor_flood = QCursor(pix, 5, 16);
  }

  pix = BarIcon("aim-cursor");
  if(pix.isNull())
  {
    cursor_aim = cursor_normal;
    kdDebug() << "KIconEditGrid: Error loading aim-cursor.xpm" << endl;
  }
  else
  {
    mask = pix.createHeuristicMask();
    pix.setMask(mask);
    cursor_aim = QCursor(pix);
  }

  pix = BarIcon("spraycan-cursor");
  if(pix.isNull())
  {
    cursor_spray = cursor_normal;
    kdDebug() << "KIconEditGrid: Error loading spraycan-cursor.xpm" << endl;
  }
  else
  {
    mask = pix.createHeuristicMask(true);
    pix.setMask(mask);
    cursor_spray = QCursor(pix);
  }

  pix = BarIcon("eraser-cursor");
  if(pix.isNull())
  {
    cursor_erase = cursor_normal;
    kdDebug() << "KIconEditGrid: Error loading eraser-cursor.xpm" << endl;
  }
  else
  {
    mask = pix.createHeuristicMask(true);
    pix.setMask(mask);
    cursor_erase = QCursor(pix, 5, 16);
  }
}



void KIconEditGrid::setTool(DrawTool t)
{
    btndown = false;
    tool = t;

    if(tool != SelectRect && tool != SelectCircle)
        isselecting = false;

    switch( tool )
    {
        case SelectRect:
            isselecting = true;
            setCursor(cursor_aim);
            break;
        case SelectCircle:
            isselecting = true;
            setCursor(cursor_aim);
            break;
        case Line:
        case Ellipse:
        case Circle:
        case FilledEllipse:
        case FilledCircle:
        case FilledRect:
        case Rect:
            setCursor(cursor_aim);
            break;
        case Freehand:
            setCursor(cursor_paint);
            break;
        case Spray:
            setCursor(cursor_spray);
            break;
        case Eraser:
            setCursor(cursor_erase);
            break;
        case FloodFill:
            setCursor(cursor_flood);
            break;
        case Find:
            setCursor(arrowCursor);
            break;
        default:
            break;
    }
}


void KIconEditGrid::drawFlood(int x, int y, uint oldcolor)
{
    if((!img->valid(x, y)) 
    || (colorAt((y * numCols())+x) != oldcolor) 
    || (colorAt((y * numCols())+x) == currentcolor))
        return;

    *((uint*)img->scanLine(y) + x) = currentcolor;
    setColor((y*numCols())+x, currentcolor, false);

    drawFlood(x, y-1, oldcolor);
    drawFlood(x, y+1, oldcolor);
    drawFlood(x-1, y, oldcolor);
    drawFlood(x+1, y, oldcolor);
}


void KIconEditGrid::drawSpray(QPoint point)
{
    int x = (point.x()-5);
    int y = (point.y()-5);

    //kdDebug() << "drawSpray() - " << x << " X " << y << endl;

    pntarray.resize(0);
    int points = 0;
    for(int i = 1; i < 4; i++, points++)
    {
        int dx = (rand() % 10);
        int dy = (rand() % 10);
        pntarray.putPoints(points, 1, x+dx, y+dy);
    }

    drawPointArray(pntarray, Draw);
}


void KIconEditGrid::drawEllipse(bool drawit)
{
    if(drawit)
    {
        drawPointArray(pntarray, Draw);
        p = *img;
        emit changed(p);
        return;
    }

    QPointArray a(pntarray.copy());
    int x = start.x(), y = start.y(), cx, cy;

    if(x > end.x())
    {
        cx = x - end.x();
        x = x - cx;
    }
    else
        cx = end.x() - x;
    if(y > end.y())
    {
        cy = y - end.y();
        y = y - cy;
    }
    else
        cy = end.y() - y;
  
    int d = (cx > cy) ? cx : cy;

    //kdDebug() << x << ", " << y << " - " << d << " " << d << endl;
    pntarray.resize(0);
    drawPointArray(a, Mark);

    if(tool == Circle || tool == FilledCircle || tool == SelectCircle)
        pntarray.makeEllipse(x, y, d, d);
    else if(tool == Ellipse || tool == FilledEllipse)
        pntarray.makeEllipse(x, y, cx, cy);

    if((tool == FilledEllipse) || (tool == FilledCircle) 
    || (tool == SelectCircle))
    {
        int s = pntarray.size();
        int points = s;
        for(int i = 0; i < s; i++)
        {
            int x = pntarray[i].x();
            int y = pntarray[i].y();
            for(int j = 0; j < s; j++)
            {
                if((pntarray[j].y() == y) && (pntarray[j].x() > x))
                {
                    for(int k = x; k < pntarray[j].x(); k++, points++)
                        pntarray.putPoints(points, 1, k, y);
                    break;
                }
            }
        }
    }

    drawPointArray(pntarray, Mark);
  
    if(tool == SelectCircle && pntarray.size() > 0 && !ispasting)
        emit selecteddata(true);
}


void KIconEditGrid::drawRect(bool drawit)
{
    if(drawit)
    {
        drawPointArray(pntarray, Draw);
        p = *img;
        emit changed(p);
        return;
    }

    QPointArray a(pntarray.copy());
    int x = start.x(), y = start.y(), cx, cy;

    if(x > end.x())
    {
        cx = x - end.x();
        x = x - cx;
    }
    else
        cx = end.x() - x;
    if(y > end.y())
    {
        cy = y - end.y();
        y = y - cy;
    }
    else
        cy = end.y() - y;

    //kdDebug() << x << ", " << y << " - " << cx << " " << cy << endl;
    pntarray.resize(0);
    drawPointArray(a, Mark); // remove previous marking

    int points = 0;
    bool pasting = ispasting;
       
    if(tool == FilledRect || (tool == SelectRect))
    {
        for(int i = x; i <= x + (pasting ? cx + 1 : cx); i++)
        {
            for(int j = y; j <= y+cy; j++, points++)
            pntarray.putPoints(points, 1, i, j);
        }
    }
    else
    {
        for(int i = x; i <= x+cx; i++, points++)
            pntarray.putPoints(points, 1, i, y);
        for(int i = y; i <= y+cy; i++, points++)
            pntarray.putPoints(points, 1, x, i);
        for(int i = x; i <= x+cx; i++, points++)
            pntarray.putPoints(points, 1, i, y+cy);
        for(int i = y; i <= y+cy; i++, points++)
            pntarray.putPoints(points, 1, x+cx, i);
    }

    drawPointArray(pntarray, Mark);
  
    if(tool == SelectRect && pntarray.size() > 0 && !ispasting)
        emit selecteddata(true);
}


void KIconEditGrid::drawLine(bool drawit, bool drawStraight)
{
    if(drawit)
    {
        drawPointArray(pntarray, Draw);
        p = *img;
        emit changed(p);
        return;
    }

    QPointArray a(pntarray.copy());
    pntarray.resize(0);

    // remove previous marking
    drawPointArray(a, Mark); 

    int x, y, dx, dy, delta; 

    dx = end.x() - start.x();
    dy = end.y() - start.y();
    x = start.x();
    y = start.y();

    delta = QMAX(abs(dx), abs(dy));
    int deltaX = abs(dx);
    int deltaY = abs(dy);

    if ((drawStraight) && (delta > 0)) 
    {
        dx /= delta;
        dy /= delta;
        
        for(int i = 0; i <= delta; i++) 
        {
            pntarray.putPoints(i, 1, x, y);
            x += dx;
            y += dy;
        }
    }

    else if ((delta > 0) && (deltaX >= deltaY)) 
    {
        for(int i = 0; i <= deltaX; i++) 
        {
            pntarray.putPoints(i, 1, x, y);
            
            if(dx > 0)
               x++;
            else
               x--; 

            if(dy >= 0)
                y = start.y() + (abs(start.x() - x) * deltaY) / deltaX;
            else    
                y = start.y() - (abs(start.x() - x) * deltaY) / deltaX;            
        }
    }
    
    else if ((delta > 0) && (deltaY > deltaX)) 
    {
        for(int i = 0; i <= deltaY; i++) 
        {
            pntarray.putPoints(i, 1, x, y);
            
            if(dy > 0)
                y++;
            else
                y--;    

            if(dx >= 0)
                x = start.x() + (abs(start.y() - y) * deltaX) / deltaY;
            else    
                x = start.x() - (abs(start.y() - y) * deltaX) / deltaY;            
        }
    }

    drawPointArray(pntarray, Mark);
}


void KIconEditGrid::drawPointArray(QPointArray a, DrawAction action)
{
    QRect rect = a.boundingRect();
    bool update = false;

    int s = a.size(); //((rect.size().width()) * (rect.size().height()));
    for(int i = 0; i < s; i++)
    {
        int x = a[i].x();
        int y = a[i].y();
    
        if(img->valid(x, y) && rect.contains(QPoint(x, y)))
        {
            //kdDebug() << "x: " << x << " - y: " << y << endl;
            switch( action )
            {
                case Draw:
                {
                    *((uint*)img->scanLine(y) + x) = currentcolor; //colors[cell]|OPAQUE;
                    int cell = y * numCols() + x;
                    setColor( cell, currentcolor, false );
                    modified = true;
                    update = true;
                    //updateCell( y, x, FALSE );
                    break;
                }
                
                case Mark:
                case UnMark:
                    repaint(x*cellsize,y*cellsize, cellsize, cellsize, false);
                    //updateCell( y, x, true );
                    break;
                    
                default:
                    break;
            }
        }
    }

    if(update)
    {
        updateColors();
        repaint(rect.x()*cellSize()-1, rect.y()*cellSize()-1,
            rect.width()*cellSize()+1, rect.height()*cellSize()+1, false);
        pntarray.resize(0);
    }

}


bool KIconEditGrid::isMarked(QPoint point)
{
    return isMarked(point.x(), point.y());
}


bool KIconEditGrid::isMarked(int x, int y)
{
    if(((y * numCols()) + x) == selected)
        return true;

    int s = pntarray.size();
    for(int i = 0; i < s; i++)
    {
        if(y == pntarray[i].y() && x == pntarray[i].x())
            return true;
    }

    return false;
}


// Fast diffuse dither to 3x3x3 color cube
// Based on Qt's image conversion functions
static bool kdither_32_to_8( const QImage *src, QImage *dst )
{
    register QRgb *p;
    uchar  *b;
    int	    y;
	
	//printf("kconvert_32_to_8\n");
	
    if ( !dst->create(src->width(), src->height(), 8, 256) ) {
		kdWarning() << "KPixmap: destination image not valid" << endl;
		return FALSE;
	}

    int ncols = 256;

    static uint bm[16][16];
    static int init=0;
    if (!init) 
    {
		// Build a Bayer Matrix for dithering
		init = 1;
		int n, i, j;

		bm[0][0]=0;

		for (n=1; n<16; n*=2) 
        {
	    	for (i=0; i<n; i++) 
            {
			    for (j=0; j<n; j++) 
                {
		    	    bm[i][j]*=4;
		    	    bm[i+n][j]=bm[i][j]+2;
		    	    bm[i][j+n]=bm[i][j]+3;
		    	    bm[i+n][j+n]=bm[i][j]+1;
			    }
	    	}
		}

		for (i=0; i<16; i++)
	    	for (j=0; j<16; j++)
			    bm[i][j]<<=8;
    }

    dst->setNumColors( ncols );

#define MAX_R 2
#define MAX_G 2
#define MAX_B 2
#define INDEXOF(r,g,b) (((r)*(MAX_G+1)+(g))*(MAX_B+1)+(b))

	int rc, gc, bc;

	for ( rc=0; rc<=MAX_R; rc++ )		// build 2x2x2 color cube
	    for ( gc=0; gc<=MAX_G; gc++ )
		    for ( bc=0; bc<=MAX_B; bc++ ) 
            {
		        dst->setColor( INDEXOF(rc,gc,bc),
			    qRgb( rc*255/MAX_R, gc*255/MAX_G, bc*255/MAX_B ) );
		    }	

	int sw = src->width();
	int* line1[3];
	int* line2[3];
	int* pv[3];

	line1[0] = new int[src->width()];
	line2[0] = new int[src->width()];
	line1[1] = new int[src->width()];
	line2[1] = new int[src->width()];
	line1[2] = new int[src->width()];
	line2[2] = new int[src->width()];
	pv[0] = new int[sw];
	pv[1] = new int[sw];
	pv[2] = new int[sw];

	for ( y=0; y < src->height(); y++ ) 
    {
	    p = (QRgb *)src->scanLine(y);
	    b = dst->scanLine(y);
		int endian = (QImage::systemByteOrder() == QImage::BigEndian);
		int x;
		uchar* q = src->scanLine(y);
		uchar* q2 = src->scanLine(y+1 < src->height() ? y + 1 : 0);
		for (int chan = 0; chan < 3; chan++) 
        {
		    b = dst->scanLine(y);
		    int *l1 = (y&1) ? line2[chan] : line1[chan];
		    int *l2 = (y&1) ? line1[chan] : line2[chan];
		    if ( y == 0 ) 
            {
			    for (int i=0; i<sw; i++)
			        l1[i] = q[i*4+chan+endian];
		    }
		    if ( y+1 < src->height() ) 
            {
			    for (int i=0; i<sw; i++)
			        l2[i] = q2[i*4+chan+endian];
		    }
		    // Bi-directional error diffusion
		    if ( y&1 ) 
            {
			    for (x=0; x<sw; x++) 
                {
			        int pix = QMAX(QMIN(2, (l1[x] * 2 + 128)/ 255), 0);
			        int err = l1[x] - pix * 255 / 2;
			        pv[chan][x] = pix;

			        // Spread the error around...
			        if ( x+1<sw ) 
                    {
				        l1[x+1] += (err*7)>>4;
				        l2[x+1] += err>>4;
			        }
			        l2[x]+=(err*5)>>4;
			        if (x>1)
				        l2[x-1]+=(err*3)>>4;
			    }
		    } 
            else 
            {
			    for (x=sw; x-->0; ) 
                {
			        int pix = QMAX(QMIN(2, (l1[x] * 2 + 128)/ 255), 0);
			        int err = l1[x] - pix * 255 / 2;
			        pv[chan][x] = pix;

			        // Spread the error around...
			        if ( x > 0 ) 
                    {
				        l1[x-1] += (err*7)>>4;
				        l2[x-1] += err>>4;
			        }
			        l2[x]+=(err*5)>>4;
			        if (x+1 < sw)
				        l2[x+1]+=(err*3)>>4;
			    }
		    }
		}
		if (endian) 
        {
		    for (x=0; x<sw; x++) 
            {
			    *b++ = INDEXOF(pv[2][x],pv[1][x],pv[0][x]);
		    }
		} 
        else 
        {
		    for (x=0; x<sw; x++) 
            {
			    *b++ = INDEXOF(pv[0][x],pv[1][x],pv[2][x]);
		    }
		}
	}

	delete line1[0];
	delete line2[0];
	delete line1[1];
	delete line2[1];
	delete line1[2];
	delete line2[2];
	delete pv[0];
	delete pv[1];
	delete pv[2];
	
#undef MAX_R
#undef MAX_G
#undef MAX_B
#undef INDEXOF

    return TRUE;
}

// this doesn't work the way it should but the way KPixmap does.
void KIconEditGrid::mapToKDEPalette()
{
    QImage dest;

    kdither_32_to_8(img, &dest);
    *img = dest.convertDepth(32);

    for(int y = 0; y < img->height(); y++)
    {
        uint *l = (uint*)img->scanLine(y);
        for(int x = 0; x < img->width(); x++, l++)
        {
            if(*l < 0xff000000)
            {
                *l = *l | 0xff000000;
            }
        }
    }

    load(img);
    return;
    
/*
#if QT_VERSION > 140
  *img = img->convertDepthWithPalette(32, iconpalette, 42);
  load(img);
  return;
#endif
*/

    QApplication::setOverrideCursor(waitCursor);
    for(int y = 0; y < numRows(); y++)
    {
        uint *l = (uint*)img->scanLine(y);
        for(int x = 0; x < numCols(); x++, l++)
        {
            if(*l != TRANSPARENT)
            {
                if(!iconcolors.contains(*l))
                    *l = iconcolors.closestMatch(*l);
            }
        }
    }

    load(img);
    modified = true;
    QApplication::restoreOverrideCursor();
}


void KIconEditGrid::grayScale()
{
    for(int y = 0; y < numRows(); y++)
    {
        uint *l = (uint*)img->scanLine(y);
        for(int x = 0; x < numCols(); x++, l++)
        {
            if(*l != TRANSPARENT)
            {
                uint c = (qBlue(*l) + qRed(*l) + qGreen(*l))/3;
                *l = QColor(c, c, c).rgb()|OPAQUE_MASK;
            }
        }
    }

    load(img);
    modified = true;
}
#include "kicongrid.moc"
