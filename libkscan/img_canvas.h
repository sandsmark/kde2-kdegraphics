/***************************************************************************
               img_canvas.h  - Class to display a scrollable image 
                             -------------------                                         
    begin                : Sun Dec 12 1999                                           
    copyright            : (C) 1999 by Klaas Freitag                         
    email                : freitag@suse.de

    $Id: img_canvas.h 105988 2001-07-13 20:35:06Z freitag $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __IMG_CANVAS_H__
#define __IMG_CANVAS_H__

#include <qwidget.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qrect.h>
#include <stdlib.h>
#include <qsize.h>
#include <qwmatrix.h>
#include <qscrollview.h>
#include <qstrlist.h>
#include <qpopupmenu.h>

#ifdef USE_KPIXMAPIO
#include <kpixmapio.h>
#endif

enum preview_state {
	MOVE_NONE,
	MOVE_TOP_LEFT,
	MOVE_TOP_RIGHT,
	MOVE_BOTTOM_LEFT,
	MOVE_BOTTOM_RIGHT,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_TOP,
	MOVE_BOTTOM,
	MOVE_WHOLE
};

enum cursor_type {
	CROSS,
	VSIZE,
	HSIZE,
	BDIAG,
	FDIAG,
	ALL,
	HREN
};

const int MIN_AREA_WIDTH = 3;
const int MIN_AREA_HEIGHT = 3;
const int delta = 3;
#ifdef __PREVIEW_CPP__
int max_dpi = 600;
#else
extern int max_dpi;
#endif



class ImageCanvas: public QScrollView
{
   Q_OBJECT
   Q_ENUMS( PopupIDs )
   Q_PROPERTY( int brightness READ getBrightness WRITE setBrightness )
   Q_PROPERTY( int contrast READ getContrast WRITE setContrast )
   Q_PROPERTY( int gamma READ getGamma WRITE setGamma )
   Q_PROPERTY( int scale_factor READ getScaleFactor WRITE setScaleFactor )
    
public:
   ImageCanvas( QWidget *parent = 0,
		const QImage *start_image = 0,
		const char *name = 0);
   ~ImageCanvas( );

   int getBrightness() const;
   int getContrast() const;
   int getGamma() const;

   int getScaleFactor() const;
   
   const QImage *rootImage();

   bool hasImage( void ) 	{ return acquired; }

   QRect sel( void );
   

   enum{ ID_POP_ZOOM, ID_POP_CLOSE, ID_FIT_WIDTH,
	    ID_FIT_HEIGHT, ID_ORIG_SIZE } PopupIDs;
				    
   bool selectedImage( QImage* );
   
public slots:
   void setBrightness(int);
   void setContrast(int );
   void setGamma(int );

   void toggleAspect( int aspect_in_mind )
      {
	 maintain_aspect = aspect_in_mind;
	 repaint();
      }	
   QSize sizeHint() const;

   void newImage( QImage* );
   void deleteView( QImage *);
   void newRectSlot();
   void newRectSlot( QRect newSel );
   void noRectSlot( void );
   void setScaleFactor( int i );
   void handle_popup(int item );

   signals:
   void noRect( void );
   void newRect( void );
   void newRect( QRect );
   void scalingRequested();
   void closingRequested();

protected:
   void drawContents( QPainter * p, int clipx, int clipy, int clipw, int cliph );

   void timerEvent(QTimerEvent *);
   void viewportMousePressEvent(QMouseEvent *);
   void viewportMouseReleaseEvent(QMouseEvent *);
   void viewportMouseMoveEvent(QMouseEvent *);
	
   void resizeEvent( QResizeEvent * event );
	
private:
   QStrList      urls;

   int           scale_factor;
   const QImage        *image;
   int           brightness, contrast, gamma;

   
#ifdef USE_KPIXMAPIO
   KPixmapIO	 pixIO;
#endif
   void          createContextMenu( void );
   void          showContextMenu( QPoint p );
   
   QWMatrix	 scale_matrix;
   QWMatrix	 inv_scale_matrix;
   QPixmap       *pmScaled;
   float	 used_yscaler;
   float	 used_xscaler;
   QPopupMenu    *contextMenu;
   bool		 maintain_aspect;

   int	         timer_id;
   QRect 	 *selected;
   preview_state moving;
   int 		 cr1,cr2;
   int 		 lx,ly;
   bool 	 acquired;
   
   
   /* private functions for the running ant */
   void drawHAreaBorder(QPainter &p,int x1,int x2,int y,int r = FALSE);
   void drawVAreaBorder(QPainter &p,int x,int y1,int y2,int r = FALSE);
   void drawAreaBorder(QPainter *p,int r = FALSE);
   void update_scaled_pixmap( void );
	
   preview_state classifyPoint(int x,int y);

   class ImageCanvasPrivate;
   ImageCanvasPrivate *d;
};

#endif
