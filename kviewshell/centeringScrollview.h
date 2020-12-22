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

#ifndef CENTERINGSCROLLVIEW_H
#define CENTERINGSCROLLVIEW_H

#include <qscrollview.h>

/**
 * CenteringScrollview is a customized QScrollView, which can hold one
 * page. This page will be centered on the viewport.
 */
class CenteringScrollview : public QScrollView
{
    Q_OBJECT

public:
    CenteringScrollview( QWidget* parent = 0, const char* name = 0 );
    ~CenteringScrollview() { ; }
    
    void addChild( QWidget* );
    QWidget* page() const { return _page; }
   
    /** Return true if the top resp. bottom of the page is visible. */
    bool atTop()    const;
    bool atBottom() const;
    
    /** Turn the scrollbars on/off. */
    void enableScrollBars( bool);

    /** If the viewport is larger than the page, center the page on
	the viewport. */
    void centerContents();
    
    /** Reimplementd from QScrollView to provide automatic
	centering */
    void resizeContents(int, int);

public slots:
    bool readUp();
    bool readDown();
    void scrollUp();
    void scrollDown();
    void scrollRight();
    void scrollLeft();
    void scrollBottom();
    void scrollTop();
   
signals:
    void viewSizeChanged( QSize size );
    void pageSizeChanged( QSize size );
    
protected:
    void keyPressEvent( QKeyEvent* );
    
    /** Reimplemented from QScrollView to make sure that the page is
	centered when it fits in the viewport. */
    void viewportResizeEvent( QResizeEvent* );

private:
    QPoint   _dragGrabPos;
    QWidget* _page;

    /** Flag to avoid infinite recursion in the re-implementation of
	resizeContents(). See the noted in the centerContents()
	method. */
    bool     centeringContents;
};

#endif
