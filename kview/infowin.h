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

#ifndef INFOWIN_H
#define INFOWIN_H

#include "infowin_base.h"

class InfoWin : public InfoWin_Base
{
  Q_OBJECT
public:
  InfoWin( QWidget* parent = 0, const char* name = "Info Window", WFlags fl = WType_TopLevel | WStyle_Dialog );
  ~InfoWin();

  void setDimensions( int, int );
  void setSize( uint );
  void setIsWritable( bool );
  void setLastModified( const QString & );
  void setDepth( int );

protected slots:
  void slot_close();
};

#endif //INFOWIN_H
