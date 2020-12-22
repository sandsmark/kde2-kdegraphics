/*  This file is part of the KDE project
    Copyright (C) 2001 Matthias Kretz <kretz@kde.org>

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

#include "infowin.h"

#include <qstring.h>
#include <qlabel.h>
#include <kglobal.h>
#include <klocale.h>
#include <kapp.h>

InfoWin::InfoWin( QWidget* parent,  const char* name, WFlags fl )
        : InfoWin_Base( parent, name, fl )
{
  kapp->setTopWidget( this );
}

InfoWin::~InfoWin()
{
}

void InfoWin::setDimensions( int x, int y )
{
  QString dimensions = QString::number( x ) + " x " + QString::number( y );
  m_dimensions->setText( dimensions );
}

void InfoWin::setSize( uint bytes )
{
  QString size = QString::number( bytes ) + " " + i18n( "Bytes" );
  m_size->setText( size );
}

void InfoWin::setIsWritable( bool b )
{
  m_writable->setText( b ? i18n( "yes" ) : i18n( "no" ) );
}

void InfoWin::setLastModified( const QString &mod )
{
  m_modified->setText( mod );
}

void InfoWin::setDepth( int d )
{
  m_depth->setText( QString::number( d ) + " " + i18n( "Bit" ) );
}

void InfoWin::slot_close()
{
  hide();
}

#include "infowin.moc"
