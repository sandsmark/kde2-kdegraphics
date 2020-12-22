/*
    KDE Draw - a small graphics drawing program for the KDE.
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

#ifndef __KRESIZE_H__
#define __KRESIZE_H__

//#include <qwidget.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qfiledialog.h> 
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlistbox.h>

#include <kiconloader.h>
#include <kapp.h>

class KIntSpinBox;

class KResizeWidget : public QWidget
{
  Q_OBJECT

public:

  KResizeWidget( QWidget* parent, const char* name, const QSize& );
  ~KResizeWidget();

  const QSize getSize();

signals:

  void validSize( bool );

public slots:

  void checkValue( int );

private:

  KIntSpinBox *x_line;
  KIntSpinBox *y_line;
};

class KResizeDialog : public QDialog
{
  Q_OBJECT
  
public:

  KResizeDialog( QWidget* parent, const char* name, const QSize s );
  ~KResizeDialog();

  const QSize getSize();

public slots:

  void validSize( bool );
  void checkValue( int );

protected:

  bool eventFilter( QObject*, QEvent* );

private:

  KResizeWidget* rw;
  QPushButton *ok, *cancel;
};

#endif //__KRESIZE_H__
