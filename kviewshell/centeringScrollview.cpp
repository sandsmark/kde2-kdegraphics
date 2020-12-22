/* This file is part of the KDE project
   Copyright (C) 2001 Wilco Greven <greven@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>

#include "centeringScrollview.h"

CenteringScrollview::CenteringScrollview( QWidget* parent, const char* name )
  : QScrollView( parent, name )
{
    _page = 0;
    centeringContents = false;    
    setFocusPolicy( QWidget::StrongFocus );
    viewport()->setFocusPolicy( QWidget::WheelFocus );
}

void CenteringScrollview::addChild( QWidget* page )
{
  if( page != 0 ) {
    QScrollView::addChild( page );
    centerContents();
    _page = page;
  }
}

bool CenteringScrollview::atTop() const
{
    return verticalScrollBar()->value() == verticalScrollBar()->minValue();
}

bool CenteringScrollview::atBottom() const 
{
    return verticalScrollBar()->value() == verticalScrollBar()->maxValue();
}

bool CenteringScrollview::readUp()
{
  if( atTop() )
    return false;
  else {
    int newValue = QMAX( verticalScrollBar()->value() - height() + 50,
			 verticalScrollBar()->minValue() );
    verticalScrollBar()->setValue( newValue );
    return true;
  }
}

bool CenteringScrollview::readDown()
{   
  if( atBottom() )
    return false;
  else {
    int newValue = QMIN( verticalScrollBar()->value() + height() - 50,
			 verticalScrollBar()->maxValue() );
    verticalScrollBar()->setValue( newValue );
    return true;
  }
}

void CenteringScrollview::scrollRight()
{
    horizontalScrollBar()->addLine();
}

void CenteringScrollview::scrollLeft()
{
    horizontalScrollBar()->subtractLine();
}

void CenteringScrollview::scrollDown()
{
    verticalScrollBar()->addLine();
}

void CenteringScrollview::scrollUp()
{
    verticalScrollBar()->subtractLine();
}

void CenteringScrollview::scrollBottom()
{
    verticalScrollBar()->setValue( verticalScrollBar()->maxValue() );
}

void CenteringScrollview::scrollTop()
{
    verticalScrollBar()->setValue( verticalScrollBar()->minValue() );
}

void CenteringScrollview::enableScrollBars( bool b )
{
    setHScrollBarMode( b ? Auto : AlwaysOff );
    setVScrollBarMode( b ? Auto : AlwaysOff );
}

void CenteringScrollview::keyPressEvent( QKeyEvent* e )
{   
    switch ( e->key() ) {
    case Key_Up:
	scrollUp();
	break;
    case Key_Down:
	scrollDown();
	break;
    case Key_Left:
	scrollLeft();
	break;
    case Key_Right:
	scrollRight();
	break;
    default:
	e->ignore();
	return;
    }
    e->accept();
}

void CenteringScrollview::viewportResizeEvent( QResizeEvent* e )
{
  QScrollView::viewportResizeEvent( e );
  emit viewSizeChanged( viewport()->size() );
  centerContents();
}

void CenteringScrollview::centerContents()
{
  if( !_page ) 
    return;
  
  int newX = 0;
  int newY = 0;

  QSize newViewportSize = viewportSize( _page->width(), _page->height() );
    
  if( newViewportSize.width() > _page->width() )
    newX = ( newViewportSize.width() - _page->width() )/2;
  if( newViewportSize.height() > _page->height() )
    newY = ( newViewportSize.height() - _page->height() )/2;

  // Note: setting the centeringContents flag here is absolutely
  // necessary to prevent infinite recursion (with stack overflow and
  // crash) when the overloaded version of resizeContents() is called.
  centeringContents = true;
  moveChild( _page, newX, newY );
  centeringContents = false;
}

void CenteringScrollview::resizeContents(int w, int h)
{
  QScrollView::resizeContents(w,h);
  if (!centeringContents)
    centerContents();
}

#include "centeringScrollview.moc"
