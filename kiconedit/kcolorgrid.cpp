/*
    kiconedit - a small graphics drawing program for creating KDE icons
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

#include <kdebug.h>

#include "kcolorgrid.h"
#include "utils.h"

void KColorArray::remove(int idx)
{
  int oldsize = size();
  if(idx >= (int)size())
  {
    kdWarning() << "KColorArray::remove: Index " << idx << " out of range" << endl;
    return;
  }
  KColorArray tmp(*this);
  tmp.detach();
  resize(size()-1);
  for(int i = idx; i < oldsize-1; i++)
    at(i) = tmp[i+1];
  //kdDebug() << "KColorArray::remove() " << at(idx) << "\t-\tsize: " << size() << endl;
}

void KColorArray::append(uint c)
{
  resize(size()+1);
  at(size()-1) = c;
  //kdDebug() << "KColorArray::append() " << c << "\t-\tsize: " << size() << endl;
}

uint KColorArray::closestMatch(uint color)
{
  //kdDebug() << "KColorArray: " << c << endl;
  uint c = color & ~OPAQUE_MASK, d = 0xffffff, t;
  //kdDebug() << "KColorArray: " << c << endl;
  //kdDebug() << "KColorArray: " << c|OPAQUE_MASK << endl;
  uint cb = c;
  for(uint i = 0; i < size(); i++)
  {
    if (at(i) > cb)
      t = at(i) - cb;
    else
      t = cb - at(i);
    if( t < d )
    {
      d = t;
      c = at(i);
    }
  }
  return c|OPAQUE_MASK;
}

KColorGrid::KColorGrid(QWidget *parent, const char *name, int space)
 : QWidget(parent, name)
{
  //kdDebug() << "KColorGrid - constructor" << endl;
  s = space;
  rows = cols = totalwidth = totalheight = 0;
  setCellSize(10);
  setGridState(Plain);
  setGrid(true);
  numcolors.resize(0);
  gridcolors.resize(0);

  //kdDebug() << "KColorGrid - constructor - done" << endl;
}
/*
void KColorGrid::show()
{
  //updateScrollBars();
  QWidget::show();
}
*/
void KColorGrid::paintEvent(QPaintEvent *e)
{
  //kdDebug() << "KColorGrid::paintEvent" << endl;

  //updateScrollBars();
  //QWidget::paintEvent(e);

  const QRect urect = e->rect();

  //kdDebug() << "Update rect = ( " << //urect.left() << ", " << urect.top() << ", " << urect.width() << ", " << urect.height() << " )" << endl;


  int firstcol = getX(urect.x())-1;
  int firstrow = getY(urect.y())-1;
  int lastcol  = getX(urect.right())+1;
  int lastrow  = getY(urect.bottom())+1;

  QWMatrix matrix;
  QPainter p;
  p.begin( this );

  firstrow = (firstrow < 0) ? 0 : firstrow;
  firstcol = (firstcol < 0) ? 0 : firstcol;
  lastrow = (lastrow >= rows) ? rows : lastrow;
  lastcol = (lastcol >= cols) ? cols : lastcol;
  //kdDebug() << urect.x() << " x " << urect.y() << "  -  row: " << urect.width() << " x " << urect.height() << endl;
  //kdDebug() << "col: " << firstcol << " -> " << lastcol << "  -  row: " << firstrow << " -> " << lastrow << endl;

/*
  if(this->isA("KDrawGrid"))
    kdDebug() << "KDrawGrid\n   firstcol: " << firstcol << "\n   lastcol: " << lastcol << "\n   firstrow: " << firstrow << "\n   lastrow: " << lastrow << endl;
*/
  for(int i = firstrow; i < lastrow; i++)
  {
    //if(this->isA("KDrawGrid"))
    //  kdDebug() << "Updating row " << i << endl;
    for(int j = firstcol; j < lastcol; j++)
    {
      matrix.translate( (j*cellsize), (i*cellsize) );
      p.setWorldMatrix( matrix );
      //p.setClipRect(j*cellsize, i*cellsize, cellsize, cellsize);
      paintCell(&p, i, j);
      //p.setClipping(FALSE);
      matrix.reset();
      p.setWorldMatrix( matrix );
    }
    //kapp->processEvents();
  }

  p.end();

}

/*
void KColorGrid::resizeEvent(QResizeEvent *)
{
  //kdDebug() << "resize: width:  " << width() << " - total: " << totalwidth << endl;
  //kdDebug() << "resize: height: " << height() << " - total: " << totalheight << endl;
}
*/

QSize KColorGrid::sizeHint() const
{
  return QSize(totalwidth, totalheight);
}

int KColorGrid::getY( int y )
{
  if(y > (totalheight-1))
    y = totalheight;
  if(cellsize == 1)
    return y;
  return (y/cellsize);
}

int KColorGrid::getX( int x )
{
  if( x > totalwidth-1)
    x = totalwidth;
  if(cellsize == 1)
    return x;
  return (x/cellsize);
}

const QRect KColorGrid::viewRect()
{
  //kdDebug() << "viewRect" << endl;
  const QRect r(0, 0, width(), height());
  //kdDebug() << "viewRect - " << x << " x " << y << " - " << w << " x " << h << endl;
  return r;
}

void KColorGrid::setNumRows(int n)
{
  //kdDebug() << "setNumRows" << endl;
  if(n < 0 || n == rows)
    return;

  rows = n;

  gridcolors.resize(n*numCols());
  //QTableView::setNumRows(n);
  totalheight = (n * cellsize) + 1;
  resize(totalwidth, totalheight);
  //kdDebug() << "setNumRows() - gridcolors: " << gridcolors.size() << "  size: " << numCols()*numRows() << endl;
}

void KColorGrid::setNumCols(int n)
{
  //kdDebug() << "setNumCols" << endl;
  if(n < 0)
    return;
  int on = numCols();
  KColorArray gc(gridcolors);
  gc.detach();
  //kdDebug() << "gc size: " << gc.size() << " numrows: " << numRows() << endl;
  gridcolors.resize(n*numRows());
  cols = n;

  totalwidth = (n * cellsize) + 1;
  resize(totalwidth, totalheight);
  //kdDebug() << "numRows: " << numRows() << endl;
  //kdDebug() << "gridcolor: " << gridcolors.size() << "  grid: " << numRows()*numCols() << endl;
  if(numRows() == 0)
    return;

  for(int i = 0; i < numRows(); i++)
  {
    for(int j = 0; j < on; j++)
    {
      //kdDebug() << "row " << i << " , col " << j << endl;
      if(on <= n)
      {
        //kdDebug() << (i*numCols())+j << " " << (i*on)+j << endl;
        gridcolors.at((i*numCols())+j) = gc.at((i*on)+j);
      }
      else
        break;
    }
  }

  //kdDebug() << "setNumCols() - gridcolors: " << gridcolors.size() << "  size: " << numCols()*numRows() << endl;
}

void KColorGrid::fill( uint color)
{
  gridcolors.fill(color);
  numcolors.resize(1);
  numcolors.at(0) = color;
  emit colorschanged(numcolors.size(), numcolors.data());
}

void KColorGrid::setColor( int colNum, uint col, bool update )
{
  //kdDebug() << "KColorGrid::setColor" << endl;
  uint oldcolor = gridcolors[colNum];
  gridcolors[colNum] = col;

  if(!update)
    return;

  //kdDebug() << "KColorGrid::setColor - before adding" << endl;
  if(!numcolors.contains(col))
  {
    //kdDebug() << "KColorGrid::setColor() - adding " << //  col << " - " << qRed(col) << " " << qGreen(col) << " " << qBlue(col) << endl;
    numcolors.append(col);
    //kdDebug() << "KColorGrid::setColor() - adding done " << numcolors.size()-1 << endl;
    //numcolors++;
    emit addingcolor(col);
  }

  //kdDebug() << "KColorGrid::setColor - before removing" << endl;
  if(!gridcolors.contains(oldcolor))
  {
    int idx = numcolors.find(oldcolor);
    if(idx != -1)
    {
      //kdDebug() << "KColorGrid::setColor() - removing " << //  oldcolor << " - " << qRed(oldcolor) << " " << qGreen(oldcolor) << " " << qBlue(oldcolor) << endl;
      numcolors.remove(idx);
      //kdDebug() << "KColorGrid::setColor() - removing done" << endl;
      emit colorschanged(numcolors.size(), numcolors.data());
    }
    //numcolors--;
  }

  //kdDebug() << "KColorGrid::setColor - before updateCell" << endl;
  repaint((colNum%numCols())*cellsize,(colNum/numCols())*cellsize,  cellsize, cellsize);
  //updateCell( colNum/numCols(), colNum%numCols(), false );
  //kdDebug() << "KColorGrid::setColor - after updateCell" << endl;
}

void KColorGrid::updateCell( int row, int col, bool  )
{
    //kdDebug() << "updateCell - before repaint" << endl;
  QWMatrix matrix;
  QPainter p;
  p.begin( this );
  matrix.translate( (col*cellsize), (row*cellsize) );
  p.setWorldMatrix( matrix );
  paintCell(&p, row, col);
  p.end();

}


void KColorGrid::updateColors()
{
  numcolors.resize(0);
  for(int i = 0; i < (int)gridcolors.size(); i++)
  {
    uint col = gridcolors.at(i);
    if(!numcolors.contains(col))
      numcolors.append(col);
  }
  emit colorschanged(numcolors.size(), numcolors.data());
}

void KColorGrid::setCellSize( int s )
{
  cellsize = s;
  totalwidth = (numCols() * s) + 1;
  totalheight = (numRows() * s) + 1;
  resize(totalwidth, totalheight);
  if ( isVisible() )
    repaint(viewRect(), false);
}
#include "kcolorgrid.moc"
