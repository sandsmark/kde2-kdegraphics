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

#ifndef IMAGELISTDIALOG_H
#define IMAGELISTDIALOG_H

#include "imagelistdialog_base.h"

#include <qtimer.h>
#include <kurl.h>
#include <qsortedlist.h>

class QStrList;
class KConfig;
class QListBoxItem;

class ImageListDialog : public ImageListDialog_Base
{
  Q_OBJECT

public:
  ImageListDialog( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~ImageListDialog();

  void addURL( const KURL&, bool show = true );
  void addURLList( const QStrList&, const bool show = true );

  /**
   * removes the current image from the list and closes it
   * if it was the last image it returns true else false
   */
  bool removeCurrent();

  void saveOptions( KConfig * ) const;
  void restoreOptions( const KConfig * );

  virtual void saveProperties( KConfig * );
  virtual void restoreProperties( KConfig * );

  /**
   * set whether the images should loop
   */
  virtual void setLoop( bool );
   
  /**
   * returns whether the images should loop
   */
  virtual bool loop();
   
  /**
   * set the Slideshow interval
   */
  virtual void setInterval( int );
   
  /**
   * returns the Slideshow interval
   */
  virtual int interval();

  /**
   * return whether the Slideshow is running
   */
  bool slideShowRunning() const;

  /**
   * set the filename of the current image
   * (needed when the image gets saved
   */
  void setFilename( const QString& );

  /**
   * set the URL of the current image
   * (needed when the image gets saved
   */
  void setURL( const KURL &url );

public slots:
  
  /**
   * Saves the Image List to a file
   */
  virtual void slot_save();

  /**
   * Loads the Image List from a file
   */
  virtual void slot_load();

  /**
   * Sorts the list
   */
  virtual void slot_sort();

  void slot_first();
  void slot_last();
  virtual void slot_previous();
  virtual void slot_next();
  virtual void slot_shuffle();

  /**
   * toggles the state of the Slideshow button and starts/ends
   * the slideshow
   */
  void slot_toggleSlideshow();

  void slot_pauseSlideshow();
  void slot_continueSlideshow();

signals:

  ///**
   //* emitted whenever another image gets selected so that it
   //* may be loaded into the canvas
   //*/
  //void selected( const KURL* );

  /**
   * when an image is ready to be loaded this signal tells
   * where to get it (and when of course)
   */
  void pleaseLoad( const QString&, const KURL& );

protected slots:

  /**
   * Selects image at specified index
   */
  void slot_select( QListBoxItem* );

  void slot_nextSlide();
  virtual void slot_slideshow( bool );

protected:

  virtual void dragEnterEvent( QDragEnterEvent* );
  virtual void dropEvent( QDropEvent* );

  struct ImageInfo {
    QString filename;
    QString format;
    KURL url;
    bool operator==( const ImageInfo& i1 )
    {
      return url.prettyURL() == i1.url.prettyURL();
    }

    bool operator!=( const ImageInfo& i1 )
    {
      return url.prettyURL() != i1.url.prettyURL();
    }

    bool operator>( const ImageInfo& i1 )
    {
      return url.prettyURL() > i1.url.prettyURL();
    }

    bool operator<( const ImageInfo& i1 )
    {
      return url.prettyURL() < i1.url.prettyURL();
    }
  };

  bool loadImage();

private:

  ImageListDialog& operator=( ImageListDialog& );
  ImageListDialog( const ImageListDialog& );

  QTimer        *m_slideTimer;
  int           m_slideInterval;
  bool          m_loop;
  bool          m_paused;

  //KURL::List    m_list;
  //int           m_current;

  QSortedList<ImageInfo> m_imagelist;
};

inline bool ImageListDialog::slideShowRunning() const
{
  return( m_slideTimer && m_slideTimer->isActive() );
}

#endif //IMAGELISTDIALOG_H
