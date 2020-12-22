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

#include <kapp.h>
#include <kpopupmenu.h>
#include <klocale.h>

#include "marklist.moc"

#define FLAG_WIDTH 20

MarkListTable::MarkListTable( QWidget * parent , const char * name )
        : QTableView( parent, name ), sel(-1), drag(-1), items()
{
        setFrameStyle( Panel | Sunken );
        setTableFlags( Tbl_autoVScrollBar | Tbl_snapToVGrid | 
                       Tbl_clipCellPainting);
        setLineWidth( 1 );
        setCellHeight( fontMetrics().lineSpacing()+4 );
        setNumCols( 2 );
        
        pup = new QPopupMenu(0, "pup");
        pup->insertItem( i18n("Mark current page"), this, SLOT(markSelected()) );
        pup->insertItem( i18n("Mark all pages"), this, SLOT(markAll()) );
        pup->insertItem( i18n("Mark even pages"), this, SLOT(markEven()) );
        pup->insertItem( i18n("Mark odd pages"), this, SLOT(markOdd()) );
        pup->insertItem( i18n("Toggle page marks"), this, SLOT(toggleMarks()) );
        pup->insertItem( i18n("Remove page marks"), this, SLOT(removeMarks()) );
        
        QColorGroup cg = QApplication::palette().normal();
        selectColor = QColor( cg.highlight() );
        selectTextColor = QColor( cg.highlightedText() );
        
        initPixmaps();
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
MarkListTable::insertItem ( const QString & text, int index,
			    const QString &tip)
{
  MarkListTableItem *mli = new MarkListTableItem( text, tip );
  items.insert( index, mli );
  setNumRows( items.count() );
}

void MarkListTable::setAutoUpdate ( bool enable)
{
	QTableView::setAutoUpdate( enable );
	if (enable) 
          repaint();
}

void MarkListTable::clear()
{
	QColorGroup cg = QApplication::palette().normal();
	if( backgroundColor() != cg.base() )
		setBackgroundColor(cg.base());
	items.clear();
	setNumRows(0);
	sel = -1;
	update();
}

int MarkListTable::cellWidth( int col )
{
	if( col==0 )
		return FLAG_WIDTH;
	else
		return width()-FLAG_WIDTH-2*frameWidth();
}

void
MarkListTable::paintCell( QPainter *p, int row, int col)
{
  QColorGroup cg = QApplication::palette().normal();
  
  if ( col == 0  ) 
    {
      if( items.at( row )->mark() ) 
	p->drawPixmap(0, 0, *flagPixmap);
      else
	p->drawPixmap(0, 0, *bulletPixmap);
    }
  
  if ( col == 1 ) 
    {
      int w = cellWidth( col );
      int h = cellHeight( row );
      QBrush brush(items.at(row)->select() ? selectColor : cg.base());
      
      p->fillRect(0, 0, w, h, brush);                      
      
      if ( items.at( row )->select() )
	p->setPen(selectTextColor);
      else
	p->setPen(cg.text());
      
      p->drawText( 0, 0, w, h, AlignCenter, items.at( row )->text() );
      QToolTip::add (this, QRect (0,0,w,h), items.at( row )->tip() );
    }
}

void MarkListTable::mousePressEvent ( QMouseEvent *e )
{
        int i = findRow( e->pos().y() );
	int c = findCol(e->pos().x());
        if ( i == -1 )
                return;
        MarkListTableItem *it = items.at( i );

	if ( e->button() == LeftButton )
	  {
	    switch (c)
	      {
	      case 0: //flags
		it->setMark( !it->mark() );
		updateCell( i, 0 );
		drag = i;
		break;
	      case 1: //page numbers
		select( i );
		break;
	      }
	  }
	else
	  if ( e->button() == RightButton )
	    pup->popup( mapToGlobal( e->pos() ) );
	  else
	    if ( e->button() == MidButton )
	      {
		it->setMark( !it->mark() );
		updateCell( i, 0 );
		drag = i;
	      }
}

void MarkListTable::mouseMoveEvent ( QMouseEvent *e )
{
	if (e->state() != MidButton &&
	    e->state() != LeftButton)
		return;

	int i = findRow( e->pos().y() );
	if ( i == drag || i == -1 )
		return;
	do {
		drag += i > drag ? 1 : -1;
		items.at( drag )->setMark( !items.at( drag )->mark() );
		updateCell( drag, 0 );
	} while ( i != drag );
}

QString MarkListTable::text( int index )
{
	if( index < 0 || index > (int) items.count() ) {
		printf("MarkList: Index out of range");
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
      items.at( sel )->setSelect( FALSE );
      updateCell( sel, 0 );
      updateCell( sel, 1 );
    }
  it->setSelect( TRUE );
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

void MarkListTable::markSelected()
{
	if ( sel == -1 )
		return;
	MarkListTableItem *it = items.at( sel );
	it->setMark( ! it->mark() );
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

int MarkListTable::rowHeight()
{
	return cellHeight();
}

void MarkListTable::changeMarks( int how, int which  )
{
	MarkListTableItem *it;
	QString t;

	setUpdatesEnabled( FALSE );
	for ( int i=items.count(); i-->0 ; )
	{
		if ( which )
		{
			t = items.at( i )->text();
			if ( t.toInt() % 2 == which - 1 )
				continue;
		}
		it = items.at( i );
		if ( how == 2 )
			it->setMark( ! it->mark() );
		else	it->setMark( how );
		updateCell( i, 0 );
	}
	setUpdatesEnabled( TRUE );
        repaint();
}

QStringList MarkListTable::markList()
{
	QStringList l;

	for ( int i=0 ; i < (signed) items.count(); i++ )
		if ( ( items.at( i ) )->mark() )
			l << items.at( i )->text();
	return l;
}


bool MarkListTable::isSelected(int i)
{
  if (i<0 || i >= (int)items.count())
    return false;
  return items.at(i)->mark();
}

//------------------------------------------------------------------

MarkList::MarkList( QWidget * parent, const char * name)
	: QWidget( parent, name )
{
	listTable = new MarkListTable ( this );
	listTable->setFocusProxy( this );
	markLabel = new QLabel ( this );
	markLabel->setFocusProxy( this );
	markLabel->setFrameStyle( QFrame::Panel | QFrame::Raised );
	markLabel->setLineWidth( 1 );
	markLabel->setMargin( 1 );
	markLabel->setPixmap( flagPixmap() );
	pageLabel = new QLabel ( this );
	pageLabel->setFocusProxy( this );
	pageLabel->setFrameStyle( QFrame::Panel | QFrame::Raised );
	pageLabel->setLineWidth( 1 );
	pageLabel->setMargin( 1 );
	pageLabel->setText( i18n("Page") );

	connect ( listTable, SIGNAL( selected( int ) ),
		this, SLOT( selectSig( int ) ) );

}

QString MarkList::text( int index )
{
	return listTable->text( index );
}

void MarkList::setSelectColors( QColor bg, QColor fg )
{
	listTable->selectColor = bg;
	listTable->selectTextColor = fg;
}

void
MarkList::resizeEvent( QResizeEvent * )
{
  markLabel->setGeometry( 0,0, FLAG_WIDTH, listTable->rowHeight()+4 );
  pageLabel->setGeometry( FLAG_WIDTH, 0,
			  width()-FLAG_WIDTH,  listTable->rowHeight()+4 );
  listTable->setGeometry( 0, pageLabel->height(),
			  width(), height()-(pageLabel->height()) );

}

void 
MarkList::insertItem ( const QString &text, int index,
		       const QString &tip)
{
  listTable->insertItem(text, index, tip);
}


void MarkList::setAutoUpdate ( bool enable )
{
	listTable->setAutoUpdate( enable );
}

void MarkList::clear()
{
	listTable->clear();
}

void MarkList::select(int index)
{
	listTable->select( index );
}

void MarkList::selectSig(int index)
{
	emit selected( index );
}

void MarkList::markSelected()
{
	listTable->markSelected();
}

void MarkList::markAll()
{
	listTable->markAll();
}

void MarkList::markEven()
{
	listTable->markEven();
}

void MarkList::markOdd()
{
	listTable->markOdd();
}

void MarkList::toggleMarks()
{
	listTable->toggleMarks();
}

void MarkList::removeMarks()
{
	listTable->removeMarks();
}

QStringList MarkList::markList()
{
	return listTable->markList();
}

QPixmap MarkList::flagPixmap() {
	QColorGroup cg = QApplication::palette().normal();

	QPixmap pm;
	pm.resize(16,16);
	pm.fill( cg.background() );

	int xOffset = 4;
	int yOffset = 3;

	QPainter p;
	p.begin( &pm);
	p.setPen( cg.text() );
	p.drawLine( xOffset+4, yOffset, xOffset+4, yOffset+9 );
	p.setPen( red );
	p.drawLine( xOffset+3, yOffset+1, xOffset, yOffset+4 );
	p.drawLine( xOffset+3, yOffset+1, xOffset+3, yOffset+4 );
	p.drawLine( xOffset, yOffset+4, xOffset+3, yOffset+4 );
	p.end();

	return pm;
}



