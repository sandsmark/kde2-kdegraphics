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

#ifndef KNUMDIALOG_H
#define KNUMDIALOG_H

#include "knumdialog_base.h"

class KNumDialog : public KNumDialog_Base
{
public:
  KNumDialog( QWidget* parent = 0, const char* name = "KNumDialog", bool modal = true, WFlags fl = 0 );
  ~KNumDialog();

  /**
   * Get a number from the user
   */
  bool getNum( int &num, const QString &message = QString::null );
  
  /**
   * Get a number from the user
   */
  bool getNum( double &num, const QString &message = QString::null );

};

#endif //KNUMDIALOG_H
