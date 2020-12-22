/*
    KDE Draw - a small graphics drawing program for the KDE
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

#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qkeycode.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <kbuttonbox.h>

#include <klocale.h>
#include <knuminput.h>

#include "kresize.h"

#ifdef KeyPress
#undef KeyPress
#endif

KResizeWidget::KResizeWidget( QWidget* parent, const char* name, const QSize& size )
 : QWidget( parent, name )
{
  //setBackgroundColor( red );

  QHBoxLayout* genLayout = new QHBoxLayout( this );

  QGroupBox* group = new QGroupBox( i18n( "Size:" ), this );
  genLayout->addWidget( group );

  QHBoxLayout* layout = new QHBoxLayout( group, 10 );

  //x_line = new KIntNumInput( QString::null, 1, 200, 1, 1, QString::null, 10, false, group );
  x_line = new KIntSpinBox( 1, 200, 1, 1, 10, group );
  x_line->setValue( size.width() );
  connect( x_line, SIGNAL( valueChanged( int ) ), SLOT( checkValue( int ) ) );
  layout->addWidget( x_line, 1 );

  QLabel* label = new QLabel( "X", group );
  layout->addWidget( label );

  //y_line = new KIntNumInput( QString::null, 1, 200, 1, 1, QString::null, 10, false, group);
  y_line = new KIntSpinBox( 1, 200, 1, 1, 10, group);
  y_line->setValue( size.height() );
  connect( y_line, SIGNAL( valueChanged( int ) ), SLOT( checkValue( int ) ) );
  layout->addWidget( y_line, 1 );
}

KResizeWidget::~KResizeWidget()
{
}

const QSize KResizeWidget::getSize()
{
  return QSize( x_line->value(), y_line->value() );
}

void KResizeWidget::checkValue( int )
{
  if( ( x_line->value() > 0 ) &&
      ( y_line->value() > 0 ) )
  {
    emit validSize( true );
  }
  else
  {
    emit validSize( false );
  }
}

KResizeDialog::KResizeDialog( QWidget* parent, const char* name, const QSize size )
 : QDialog( parent, name, true )
{
  setCaption(i18n("Select size:"));
  setFocusPolicy( QWidget::NoFocus );

  // TODO : Do we need this ?
  QVBoxLayout* ml = new QVBoxLayout( this, 10 );

  rw = new KResizeWidget( this, "resize widget", size );
  rw->setMinimumSize( 100, 100 );
  ml->addWidget( rw, 1 );
  connect( rw, SIGNAL( validSize( bool ) ), this, SLOT( validSize( bool ) ) );

  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  ok = bb->addButton( i18n("&OK") );
  ok->setDefault( true );
  ok->setEnabled( false );
  cancel = bb->addButton( i18n( "&Cancel" ) );
  bb->layout();
  ml->addWidget( bb);
  connect( ok, SIGNAL( clicked() ), SLOT( accept() ) );
  connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );
  checkValue( 0 );
}

KResizeDialog::~KResizeDialog()
{
}

bool KResizeDialog::eventFilter(QObject *obj, QEvent *e)
{
  if ( e->type() == QEvent::KeyPress && obj == this && !testWFlags(WType_Modal))
  {
    QKeyEvent *k = (QKeyEvent*)e;
    if(k->key() == Key_Escape || k->key() == Key_Return || k->key() == Key_Enter)
    {
      return true;
    }
  }
  return false;
}

const QSize KResizeDialog::getSize()
{
  return rw->getSize();
}

void KResizeDialog::validSize( bool valid )
{
  ok->setEnabled( valid );
}

void KResizeDialog::checkValue( int )
{
  rw->checkValue( 0 );
}
#include "kresize.moc"
