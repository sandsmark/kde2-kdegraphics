/*  This file is part of the KDE project
    Copyright (C) 2000, 2001 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "kviewconfdialog.h"

#include <qcolor.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qradiobutton.h>

#include <kapp.h>
#include <knumvalidator.h>
#include <kcolorbutton.h>
#include <kdebug.h>

KViewConfDialog::KViewConfDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : KViewConfDialog_Base( parent, name, modal, fl )
{
  kapp->setTopWidget( this );
  m_interval->setValidator( new KIntValidator( 0,9999,m_interval ) );
  m_interval->setFocus();
}

KViewConfDialog::~KViewConfDialog()
{
}

bool KViewConfDialog::loop()
{
  return m_loop->isChecked();
}

void KViewConfDialog::setLoop( bool b )
{
  m_loop->setChecked( b );
}

int KViewConfDialog::interval()
{
  return m_interval->text().toInt();
}

void KViewConfDialog::setInterval( int n )
{
  m_interval->setText( QString::number( n ) );
}

const QColor KViewConfDialog::color() const
{
  return m_colorButton->color();
}

void KViewConfDialog::setColor( const QColor & color )
{
  m_colorButton->setColor( color );
}

int KViewConfDialog::resize()
{
  int ret = 0;
  if( m_resizeWindow->isChecked() )
    ret += 1;
  if( m_resizeImage->isChecked() )
    ret += 2;
  if( m_fastscaling->isChecked() )
    ret += 4;
  return ret;
}

void KViewConfDialog::setResize( int n )
{
  m_resizeWindow->setChecked( n & 1 );
  m_resizeImage->setChecked( n & 2 );
  m_fastscaling->setChecked( n & 4 );
}

void KViewConfDialog::slotDefault()
{
  m_loop->setChecked( true );
  m_interval->setText( "5" );
  m_resizeWindow->setChecked( true );
  m_resizeImage->setChecked( true );
  m_colorButton->setColor( QColor( 0, 0, 0 ) );
  m_fastscaling->setChecked( true);
}

#include "kviewconfdialog.moc"
