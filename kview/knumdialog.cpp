/*  This file is part of the KDE project
    Copyright (C) 2000 Matthias Kretz <kretz@kde.org>

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

#include "knumdialog.h"

#include <qstring.h>
#include <klineedit.h>
#include <qlabel.h>
#include <kglobal.h>
#include <klocale.h>
#include <kapp.h>
#include <knumvalidator.h>

KNumDialog::KNumDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
        : KNumDialog_Base( parent, name, modal, fl )
{
  //setIcon( kapp->miniIcon() );
  kapp->setTopWidget( this );
  m_edit->setValidator( new KFloatValidator( m_edit ) );
  m_edit->setFocus();
}

KNumDialog::~KNumDialog()
{
}

bool KNumDialog::getNum( int &num, const QString &message )
{
  m_message->setText( message );
  m_edit->setText( KGlobal::locale()->formatNumber( num, 0 ) );

  if( exec() ) {
    num = (int)KGlobal::locale()->readNumber( m_edit->text() );
    return true;
  }

  return false;
}

bool KNumDialog::getNum( double &num, const QString &message )
{
  m_message->setText( message );
  m_edit->setText( KGlobal::locale()->formatNumber( num ) );

  if( exec() ) {
    num = KGlobal::locale()->readNumber( m_edit->text() );
    return true;
  }

  return false;
}
