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
#include <qstringlist.h>
#include <qlabel.h>
#include <qstring.h>

class MarkListTableItem
{
public:
    MarkListTableItem( const QString & s,
		       const QString & t) : marked(0), selected(0)  { _text = s; 
		       _tip = t;}
	void	setMark( bool flag )	{ marked = flag; }
	bool	mark() const		{ return marked; }
	void	setSelect( bool flag )  { selected = flag; }
	bool	select() const		{ return selected; }
	QString text() const		{ return _text; }
	QString tip() const		{ return _tip; }
    private:
	bool	marked;
	bool	selected;
	QString _text;
	QString _tip;
}; 

class MarkListTable: public QTableView
{
	Q_OBJECT

public:
	MarkListTable( QWidget * parent = 0, const char * name = 0 );
	~MarkListTable() { }
	QStringList	markList();
	void	insertItem ( const QString &text, int index=-1,
			     const QString &tip = QString::null);
	void	setAutoUpdate ( bool enable );
	void	clear();
	int	rowHeight();
	QString text( int index );
	
	QColor selectColor;
	QColor selectTextColor;

	bool isSelected(int index);

	int count() { return items.count(); };

public slots:
	void	select(int);
	void	markSelected();
	void	markAll();
	void	markEven();
	void	markOdd();
	void	toggleMarks();
	void	removeMarks();

signals:
	void	selected( int index );
	void	selected( const QString & text );

protected:
	void	mousePressEvent ( QMouseEvent* );
	void	mouseReleaseEvent ( QMouseEvent* ) {}
	void	mouseMoveEvent ( QMouseEvent* );
	void	paintCell( QPainter *p, int row, int col );
	int	cellWidth( int );
	void	updateItem( int i );
	

private:
	void	changeMarks( int, int = 0 );
        void    initPixmaps();
	QPoint	mouse;
	int	sel;
	QPopupMenu* pup;
	int	drag;
	QList<MarkListTableItem> items;
        QPixmap *flagPixmap, *bulletPixmap;
};

class MarkList: public QWidget
{
	Q_OBJECT
	
public:
	MarkList( QWidget * parent = 0, const char * name = 0 );
	~MarkList() { }
	QStringList	markList();
	void	insertItem ( const QString &text, int index=-1,
			     const QString &tip = QString::null);
	void	setAutoUpdate ( bool enable );
	void	clear();
	void 	setSelectColors( QColor bg, QColor fg );
	QString text( int index );
	
	int count() { return listTable->count(); };

	bool isSelected(int index) { return listTable->isSelected(index); };

public slots:
	void	select(int index);
	void	markSelected();
	void	markAll();
	void	markEven();
	void	markOdd();
	void	toggleMarks();
	void	removeMarks();

signals:
	void	selected( int index );
	
protected:
	void resizeEvent( QResizeEvent * );

private slots:
	void	selectSig(int index);

private:
	QLabel *markLabel;
	QLabel *pageLabel;
	MarkListTable *listTable;
	QPixmap flagPixmap();

};

#endif 
