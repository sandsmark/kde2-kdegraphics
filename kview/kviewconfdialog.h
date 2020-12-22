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

#ifndef KVIEWCONFDIALOG_H
#define KVIEWCONFDIALOG_H

#include "kviewconfdialog_base.h"

class QColor;

class KViewConfDialog : public KViewConfDialog_Base
{
  Q_OBJECT

public:
  KViewConfDialog( QWidget* parent = 0, const char* name = "KViewConfDialog", bool modal = true, WFlags fl = 0 );
  ~KViewConfDialog();

  /**
   * return whether Loop is checked
   */
  bool loop();

  /**
   * return the Slideshow interval in secs
   */
  int interval();

  /**
   * return what background color should be used
   */
  const QColor color() const;

  /**
   * set the Background color
   */
  void setColor( const QColor & );

  /**
   * return the resize option 0 - 2
   */
  int resize();

  /**
   * set whether Loop is checked
   */
  void setLoop( bool );

  /**
   * set the Slideshow interval
   */
  void setInterval( int );

  /**
   * set the resize option
   */
  void setResize( int );

protected slots:
  virtual void slotDefault();

};

#endif //KVIEWCONFDIALOG_H
