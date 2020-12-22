/****************************************************************************
**
** A simple widget to mark and select entries in a list.
**
** Copyright (C) 1997 by Markku Hihnala.
** This class is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <qtabdialog.h>
#include <qmultilineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <stdio.h>
#include <qstring.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qtooltip.h>
#include <qscrollbar.h>

#include <kapp.h>
#include <kaction.h>
#include <kpopupmenu.h>
#include <klocale.h>

#include "marklist.moc"

#define FLAG_WIDTH 20

MarkListTable::MarkListTable( QWidget * parent , const char * name )
   : QTableView( parent, name ), sel( -1 ), drag( -1 ), items()
{
    setFrameStyle( Panel | Sunken );
    setLineWidth( 1 );
    setTableFlags( Tbl_autoVScrollBar | Tbl_snapToVGrid | 
		   Tbl_clipCellPainting);
    setCellHeight( fontMetrics().lineSpacing() + 4 );
    setNumCols( 2 );
       
    _markCurrent    = new KAction( i18n( "Mark Current Page" ), 0, 
				   this, SLOT( markCurrent() ), this );
    _markAll	    = new KAction( i18n( "Mark &All Pages" ), 0,
				   this, SLOT( markAll() ), this );
    _markEven	    = new KAction( i18n( "Mark &Even Pages" ), 0,
				   this, SLOT( markEven() ), this );
    _markOdd	    = new KAction( i18n( "Mark &Odd Pages" ), 0,
				   this, SLOT( markOdd() ), this );
    _toggleMarks    = new KAction( i18n( "&Toggle Page Marks"), 0,
				   this, SLOT( toggleMarks() ), this );
    _removeMarks    = new KAction( i18n( "&Remove Page Marks" ), 0,
				   this, SLOT( removeMarks() ), this );

    _actionMenu = new KActionMenu( i18n( "&Page Marks" ), this );
    _actionMenu->insert( _markCurrent );
    _actionMenu->insert( _markAll );
    _actionMenu->insert( _markEven );
    _actionMenu->insert( _markOdd );
    _actionMenu->insert( _toggleMarks );
    _actionMenu->insert( _removeMarks );
        
    initPixmaps();
}

MarkListTable::~MarkListTable()
{
  delete flagPixmap;
  delete bulletPixmap;
}


void MarkListTable::initPixmaps() 
{
        QColorGroup cg = QApplication::palette().normal();
  
        int w = cellWidth(0);
        int h = cellHeight(0);
        int xOffset, yOffset;
        
        flagPixmap = new QPixmap(w, h);
        bulletPixmap =new QPixmap(w, h);
        
        QPainter p;
        QBrush brush(cg.base());
        
        p.begin( flagPixmap );
        
        xOffset=6;
        yOffset=3;
        
        p.fillRect(0, 0, w, h, brush); 
        p.drawLine( xOffset+4, yOffset, xOffset+4, yOffset+9 );
        p.setPen( red );
        p.drawLine( xOffset+3, yOffset+1, xOffset, yOffset+4 );
        p.drawLine( xOffset+3, yOffset+1, xOffset+3, yOffset+4 );
        p.drawLine( xOffset, yOffset+4, xOffset+3, yOffset+4 );
        
        p.end();
        
        p.begin( bulletPixmap );
        
        xOffset=4; 
        yOffset=5;        
        
        p.fillRect(0, 0, w, h, brush); 
        p.setPen( cg.dark() );
        p.setBrush( cg.dark() );
        p.drawEllipse( xOffset+4, yOffset, 4, 4 );
        p.setPen( white );
        p.drawPoint( xOffset+5, yOffset+1);
        
        p.end();
}


void	
MarkListTable::insertItem ( const QString& text, int index,
			    const QString& tip)
{
    MarkListTableItem* mli = new MarkListTableItem( text, tip );
    items.insert( index, mli );
    setNumRows( items.count() );
}

void MarkListTable::setAutoUpdate ( bool enable)
{
    QTableView::setAutoUpdate( enable );
    if( enable ) 
	repaint();
}

void MarkListTable::clear()
{
    QColorGroup cg = QApplication::palette().normal();
    if( backgroundColor() != cg.base() )
	setBackgroundColor(cg.base());
    items.clear();
    sel = -1;
    update();
}

int MarkListTable::cellWidth( int col )
{
    if( col == 0 )
	return FLAG_WIDTH;
    else
	return width() - FLAG_WIDTH - 2*frameWidth();
}

void MarkListTable::paintCell( QPainter* p, int row, int col )
{
    QColorGroup cg = QApplication::palette().normal();
  
    if( col == 0 ) {
	if( items.at( row )->isMarked() ) 
	    p->drawPixmap( 0, 0, *flagPixmap );
	else
	    p->drawPixmap( 0, 0, *bulletPixmap );
    }
  
    if( col == 1 ) {
	int w = cellWidth( col );
	int h = cellHeight( row );
	QBrush brush( items.at( row )->isSelected() 
		      ? cg.highlight() : cg.base() );
      
	p->fillRect( 0, 0, w, h, brush );
      
	if( items.at( row )->isSelected() )
	    p->setPen( cg.highlightedText() );
	else
	    p->setPen(cg.text());
      
	p->drawText( 0, 0, w, h, AlignCenter, items.at( row )->text() );
	QToolTip::add( this, QRect(0,0,w,h), items.at( row )->tip() );
    }
}

void MarkListTable::mousePressEvent( QMouseEvent* e )
{
    int row = findRow( e->pos().y() );
    int col = findCol( e->pos().x() );
    
    if ( row == -1 )
	return;
    
    MarkListTableItem* it = items.at( row );

    if( e->button() == LeftButton ) {
	switch( col ) {
	case 0: //flags
	    it->setMarked( !it->isMarked() );
	    updateCell( row, 0 );
	    drag = row;
	    break;
	case 1: //page numbers
	    select( row );
	    drag = -1;
	    break;
	}
    }
    else if( e->button() == MidButton ) {
	it->setMarked( !it->isMarked() );
	updateCell( row, 0 );
	drag = row;
    }
    else if( e->button() == RightButton )
	_actionMenu->popup( mapToGlobal( e->pos() ) );
}

void MarkListTable::mouseMoveEvent( QMouseEvent* e )
{
    if( e->state() != MidButton && e->state() != LeftButton || drag == -1 )
	return;

    int row = findRow( e->pos().y() );
    if( row == drag || row == -1 )
	return;
	
    do {
	drag += row > drag ? 1 : -1;
	items.at( drag )->setMarked( !items.at( drag )->isMarked() );
	updateCell( drag, 0 );
    } while ( row != drag );
}

QString MarkListTable::text( int index )
{
    if( index < 0 || index > (int) items.count() ) {
	  //printf("MarkList: Index out of range");
		return 0;
    }
        
    MarkListTableItem *it = items.at( index );
    return it->text();
}

void
MarkListTable::select( int i )
{
  if ( i < 0 || i >= (signed) items.count() || i == sel )
    return ;
  
  MarkListTableItem *it = items.at( i );
  if ( sel != -1 )
    {
      items.at( sel )->setSelected( FALSE );
      updateCell( sel, 0 );
      updateCell( sel, 1 );
    }

  it->setSelected( TRUE );
  sel = i;
  updateCell( i, 0 );
  updateCell( i, 1 );
  emit selected( i );
  emit selected( it->text() );

  if ( ( i<=0 || rowIsVisible( i-1 ) ) &&
       ( i>= (signed) items.count()-1 || rowIsVisible( i+1 ) ) )
    return;
  setTopCell( QMAX( 0, i - viewHeight()/cellHeight()/2 ) );
}

void MarkListTable::markCurrent()
{
	if ( sel == -1 )
		return;
	MarkListTableItem *it = items.at( sel );
	it->setMarked( ! it->isMarked() );
	updateCell( sel, 0 );
}

void MarkListTable::markAll()
{
	changeMarks( 1 );
}

void MarkListTable::markEven()
{
	changeMarks( 1, 2 );
}

void MarkListTable::markOdd()
{
	changeMarks( 1, 1 );
}


void MarkListTable::removeMarks()
{
	changeMarks( 0 );
}

void MarkListTable::toggleMarks()
{
	changeMarks( 2 );
}

int MarkListTable::rowHeight() const
{
	return cellHeight();
}

void MarkListTable::changeMarks( int how, int which  )
{
	MarkListTableItem *it;
	QString t;

	setUpdatesEnabled( FALSE );
	for( int i = 0; i < (signed) items.count(); ++i )
	{
		if ( which )
		{
			if( (i + 1) % 2 == which - 1 )
				continue;
		}
		it = items.at( i );
		if ( how == 2 )
			it->setMarked( ! it->isMarked() );
		else	it->setMarked( how );
		updateCell( i, 0 );
	}
	setUpdatesEnabled( TRUE );
        repaint();
}

QValueList<int> MarkListTable::markList() const
{
	QValueList<int> l;
	QListIterator< MarkListTableItem > it( items );
	unsigned int i = 1;

	for( ; it.current(); ++it, ++i ) {	
	    if( it.current()->isMarked() )
		l.append( i );
	}
		
	return l;
}

//------------------------------------------------------------------

MarkList::MarkList( QWidget* parent, const char* name)
  : QWidget( parent, name )
{
    _table = new MarkListTable ( this );
    _table->setFocusProxy( this );
    _markLabel = new QLabel ( this );
    _markLabel->setFocusProxy( this );
    _markLabel->setFrameStyle( QFrame::Panel | QFrame::Raised );
    _markLabel->setLineWidth( 1 );
    _markLabel->setMargin( 1 );
    _markLabel->setPixmap( flagPixmap() );
    _pageLabel = new QLabel ( this );
    _pageLabel->setFocusProxy( this );
    _pageLabel->setFrameStyle( QFrame::Panel | QFrame::Raised );
    _pageLabel->setLineWidth( 1 );
    _pageLabel->setMargin( 1 );
    _pageLabel->setText( i18n("Page") );

    connect( _table, SIGNAL( selected( int ) ), 
	     this,   SIGNAL( selected( int ) ) );
}

void
MarkList::resizeEvent( QResizeEvent* )
{
    _markLabel->setGeometry( 0, 0, FLAG_WIDTH, _table->rowHeight() + 4 );
    _pageLabel->setGeometry( FLAG_WIDTH, 0,
			     width() - FLAG_WIDTH, _table->rowHeight() + 4 );
    _table->setGeometry( 0, _pageLabel->height(),
			 width(), height() - _pageLabel->height() );

}

void MarkList::wheelEvent( QWheelEvent* e )
{
    if( _table->verticalScrollBar() )
	QApplication::sendEvent( _table->verticalScrollBar(), e );
}

QPixmap MarkList::flagPixmap() 
{
    QColorGroup cg = QApplication::palette().normal();

    QPixmap pm;
    pm.resize(16,16);
    pm.fill( cg.background() );

    int xOffset = 4;
    int yOffset = 3;

    QPainter p;
    p.begin( &pm );
    p.setPen( cg.text() );
    p.drawLine( xOffset + 4, yOffset,     xOffset + 4, yOffset + 9 );
    p.setPen( red );
    p.drawLine( xOffset + 3, yOffset + 1, xOffset,     yOffset + 4 );
    p.drawLine( xOffset + 3, yOffset + 1, xOffset + 3, yOffset + 4 );
    p.drawLine( xOffset,     yOffset + 4, xOffset + 3, yOffset + 4 );
    p.end();

    return pm;
}
