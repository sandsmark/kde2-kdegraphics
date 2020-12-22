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

#ifndef _INFO_DIALOG_H_
#define _INFO_DIALOG_H_

class QLabel;

#include <kdialogbase.h>

class InfoDialog : public KDialogBase 
{
  Q_OBJECT

  public:
    InfoDialog( QWidget *parent=0, const char *name=0, bool modal=true );
    void setup( const QString &fileName, const QString &documentTitle,
		const QString &publicationDate );

  private:
    QLabel *mFileLabel;
    QLabel *mTitleLabel;
    QLabel *mDateLabel;
};

#endif
