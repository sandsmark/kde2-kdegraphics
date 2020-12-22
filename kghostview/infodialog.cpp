/*
 *   kghostview: KDE PostScript viewer
 *   This file: Copyright (C) 2000 Espen Sand, espen@kde.org
 *   Uses ideas from ealier dialog code by Mark Donohoe and Tim Theisen
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
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

#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>

#include <klocale.h>

#include "infodialog.h"

//
// Using KDialogBase in message box mode (gives centered action button)
//
InfoDialog::InfoDialog( QWidget *parent, const char *name, bool modal )
  :KDialogBase( i18n("Document information"), Yes, Yes, Yes, parent,
		name, modal, true, i18n("&OK") )
{
  QFrame *page = makeMainWidget();
  QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );
  QGridLayout *glay = new QGridLayout( topLayout, 3, 2 );
  glay->setColStretch(1,1);

  QLabel *label = new QLabel( i18n("File name:" ), page );
  glay->addWidget( label, 0, 0, AlignRight|AlignVCenter );
  mFileLabel = new QLabel( page );
  glay->addWidget( mFileLabel, 0, 1 );

  label = new QLabel( i18n("Document title:" ), page );
  glay->addWidget( label, 1, 0, AlignRight|AlignVCenter );
  mTitleLabel = new QLabel( page );
  glay->addWidget( mTitleLabel, 1, 1 );

  label = new QLabel( i18n("Publication date:" ), page );
  glay->addWidget( label, 2, 0, AlignRight|AlignVCenter );
  mDateLabel = new QLabel( page );
  glay->addWidget( mDateLabel, 2, 1 );

  topLayout->addStretch(1);
}


void InfoDialog::setup( const QString &fileName, const QString &documentTitle,
			const QString &publicationDate )
{
  mFileLabel->setText( fileName );
  mTitleLabel->setText( documentTitle );
  mDateLabel->setText( publicationDate );
}

#include "infodialog.moc"
