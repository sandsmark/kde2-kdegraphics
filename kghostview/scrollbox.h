/****************************************************************************
**
** A simple widget to control scrolling in two dimensions.
**
** Copyright (C) 1997 by Markku Hihnala. 
** This class is freely distributable under the GNU Public License.
**
*****************************************************************************/

#ifndef SCROLL_BOX_H
#define SCROLL_BOX_H

#include <qframe.h>

class ScrollBox: public QFrame
{
	Q_OBJECT

public:
	ScrollBox( QWidget * parent = 0, const char * name = 0 );

public slots:
	void	setPageSize( QSize );
	void	setViewSize( QSize );
	void	setViewPos( QPoint );
	void	setViewPos( int x, int y ) { setViewPos( QPoint( x, y ) ); }

signals:
	void	valueChanged( QPoint );
	void    valueChangedRelative( int dx, int dy );
	void	button2Pressed(); 
	void	button3Pressed(); 

protected:
	void	mousePressEvent ( QMouseEvent *);
	void	mouseMoveEvent ( QMouseEvent *);
	void	drawContents ( QPainter *);

private:
	void    setBackgroundMode();

private:
	QPoint	viewpos, mouse;
	QSize	pagesize;
	QSize	viewsize;
};

#endif
