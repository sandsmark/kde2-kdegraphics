/****************************************************************************
**
** A simple widget to mark and select entries in a list.
**
** Copyright (C) 1997 by Markku Hihnala. 
** This class is freely distributable under the GNU Public License.
**
*****************************************************************************/

#ifndef MARKLIST_H
#define MARKLIST_H

#include <qtableview.h>
#include "kpopupmenu.h"
#include <qstrlist.h>
#include <qlabel.h>
#include <qstring.h>

class KAction;
class KActionMenu;

class MarkListTableItem
{
public:
    MarkListTableItem( const QString& text, const QString& tip) 
      : _marked(0), _selected(0), _text( text ), _tip( tip ) {}
    
    void    setMarked( bool flag )	{ _marked = flag; }
    bool    isMarked() const		{ return _marked; }
    void    setSelected( bool flag )	{ _selected = flag; }
    bool    isSelected() const		{ return _selected; }
    QString text() const		{ return _text; }
    QString tip() const			{ return _tip; }
    
private:
    bool    _marked;
    bool    _selected;
    QString _text;
    QString _tip;
}; 

class MarkListTable: public QTableView
{
    Q_OBJECT

public:
    friend class MarkList;
	
    MarkListTable( QWidget* parent = 0, const char* name = 0 );
    ~MarkListTable();
    QValueList<int> markList() const;
    void    insertItem( const QString& text, int index = -1,
			const QString& tip = QString::null );
    void    setAutoUpdate( bool enable );
    void    clear();
    int	    rowHeight() const;
    QString text( int index );
	
public slots:
    void    select( int );
    void    markCurrent();
    void    markAll();
    void    markEven();
    void    markOdd();
    void    toggleMarks();
    void    removeMarks();

signals:
    void    selected( int index );
    void    selected( const QString & text );

protected:
    void    mousePressEvent ( QMouseEvent* );
    void    mouseReleaseEvent ( QMouseEvent* ) {}
    void    mouseMoveEvent ( QMouseEvent* );
    void    paintCell( QPainter *p, int row, int col );
    int	    cellWidth( int );
    void    updateItem( int i );
	
private:
    KAction* _markCurrent;
    KAction* _markAll;
    KAction* _markEven;
    KAction* _markOdd;
    KAction* _toggleMarks;
    KAction* _removeMarks;

    KActionMenu* _actionMenu;
	
    void    changeMarks( int, int = 0 );
    void    initPixmaps();
    QPoint  mouse;
    int	    sel;
    int	    drag;
    QList< MarkListTableItem > items;
    QPixmap *flagPixmap, *bulletPixmap;
};

class MarkList: public QWidget
{
    Q_OBJECT
	
public:
    MarkList( QWidget* parent = 0, const char* name = 0 );
    
    QValueList<int> markList() const	{ return _table->markList(); }
    QString text( int index )		{ return _table->text( index ); }
    void    clear()			{ _table->clear(); }
    void    setAutoUpdate ( bool enable )
    { _table->setAutoUpdate( enable ); }
    
    void    insertItem( const QString& text, int index = -1,
			const QString& tip = QString::null )
    { _table->insertItem( text, index, tip); }
    
public slots:
    void    select( int index )	{ _table->select( index ); }
    void    markCurrent()	{ _table->markCurrent(); }
    void    markAll()		{ _table->markAll(); }
    void    markEven()		{ _table->markEven(); }
    void    markOdd()		{ _table->markOdd(); }
    void    toggleMarks()	{ _table->toggleMarks(); }
    void    removeMarks()	{ _table->removeMarks(); }

signals:
    void    selected( int index );
	
protected:
    void    resizeEvent( QResizeEvent* );
    void    wheelEvent( QWheelEvent* );
    
    QPixmap flagPixmap();

private:
    QLabel* _markLabel;
    QLabel* _pageLabel;
    MarkListTable* _table;
};

#endif 
