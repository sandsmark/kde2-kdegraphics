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

#include "imagelistdialog.h"

#include <qtimer.h>
#include <qstrlist.h>
#include <qpushbutton.h>
#include <qdragobject.h>
#include <qsortedlist.h>
#include <qfile.h>
#include <qtextstream.h>

#include <kconfig.h>
#include <klistbox.h>
#include <kurl.h>
#include <klocale.h>
#include <kapp.h>
#include <krandomsequence.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <ktempfile.h>

ImageListDialog::ImageListDialog( QWidget* parent,  const char* name, WFlags fl )
        : ImageListDialog_Base( parent, name, fl ),
        m_slideTimer( 0 ),
        m_slideInterval( 5 ),
        m_loop( false ),
        m_paused( false )
{
  setAcceptDrops( true );

  kapp->setTopWidget( this );
}

ImageListDialog::~ImageListDialog()
{
  while( ! m_imagelist.isEmpty() )
  {
    (void)m_imagelist.first();
    KIO::NetAccess::removeTempFile( m_imagelist.current()->filename );
    m_imagelist.remove();
  }
  delete m_slideTimer;
  m_slideTimer = 0;
}

void ImageListDialog::setLoop( bool b )
{
  m_loop = b;
}

bool ImageListDialog::loop()
{
  return m_loop;
}

void ImageListDialog::setInterval( int seconds )
{
  m_slideInterval = seconds;
  if( slideShowRunning() )
    m_slideTimer->changeInterval( seconds * 1000 );
}

int ImageListDialog::interval()
{
  return m_slideInterval;
} 

bool ImageListDialog::removeCurrent()
{
  if( m_imagelist.isEmpty() ) return true;

  KIO::NetAccess::removeTempFile( m_imagelist.current()->filename );
  m_imageListBox->setCurrentItem( m_imagelist.at() );
  m_imagelist.remove();
  if( m_imagelist.current() == 0 ) (void)m_imagelist.last();

  m_imageListBox->removeItem( m_imageListBox->currentItem() );
  m_imageListBox->setCurrentItem( m_imagelist.at() );

  if( !m_imagelist.isEmpty() )
  {
    loadImage();
  }

  return m_imagelist.isEmpty();
}

void ImageListDialog::addURL( const KURL &url, bool show )
{
  if( url.isEmpty() ) return;

  ImageInfo *newimage = new ImageInfo;
  newimage->url = url;
  newimage->filename = QString::null;
  newimage->format = QString::null;
  m_imagelist.append( newimage );

  m_imageListBox->insertItem( url.prettyURL() ); //appends to the visible listbox

  if( show )
    loadImage();
}

bool ImageListDialog::loadImage()
{
  bool sliding = slideShowRunning();
  if( sliding ) slot_pauseSlideshow();
  ImageInfo *image = m_imagelist.current();
  if( 0 == image ) return false;

  if( image->url.isMalformed() )
  {
    kdWarning() << "ImageListDialog::loadImage() called with invalid url" << endl;
    return false;
  }

  if( image->filename == QString::null )
  {
    if( ! KIO::NetAccess::download( image->url, image->filename ) )
    {
      KMessageBox::error( this, i18n( "Could not load\n%1" ).arg( image->url.prettyURL() ) );
      m_imageListBox->setCurrentItem( m_imagelist.at() );
      m_imageListBox->removeItem( m_imageListBox->currentItem() );
      m_imagelist.remove();
      (void)m_imagelist.last();
      m_imageListBox->setCurrentItem( m_imagelist.at() );
      return false;
    }
  }
      
  emit pleaseLoad( image->filename, image->url );
  m_imageListBox->setCurrentItem( m_imagelist.at() );
  if( sliding ) slot_continueSlideshow();
  return true;
}

void ImageListDialog::setFilename( const QString &filename )
{
  m_imagelist.current()->filename = filename;
}

void ImageListDialog::setURL( const KURL &url )
{
  m_imagelist.current()->url = url;
  m_imageListBox->changeItem( url.prettyURL(), m_imagelist.at() );
}

void ImageListDialog::slot_first()
{
  if( m_imagelist.isEmpty() ) return;

  (void)m_imagelist.first();
  loadImage();
}

void ImageListDialog::slot_last()
{
  if( m_imagelist.isEmpty() ) return;

  (void)m_imagelist.last();
  loadImage();
}

void ImageListDialog::slot_previous()
{
  if( m_imagelist.at() == 0 )
  {
    if( m_loop ) slot_last();
    return;
  }

  (void)m_imagelist.prev();
  loadImage();
}

void ImageListDialog::slot_next()
{
  if( m_imagelist.getLast() == m_imagelist.current() )
  {
    if( m_loop )
      slot_first();
    else
      if( slideShowRunning() ) slot_slideshow( false );
  }
  else
  {
    (void)m_imagelist.next();
    loadImage();
  }
}

void ImageListDialog::dragEnterEvent( QDragEnterEvent *drag )
{
  drag->accept( 
    QUriDrag::canDecode( drag ) |
    QImageDrag::canDecode( drag )
  );
}

void ImageListDialog::dropEvent( QDropEvent *drop )
{
  QStrList list;
  QImage image;
  if( QUriDrag::decode( drop, list ) )
    addURLList( list, false );
  else if( QImageDrag::decode( drop, image ) )
  {
    KTempFile tempfile;
    QString filename = tempfile.name();
    tempfile.close();

    image.save( filename, "JPG" );

    addURL( KURL( filename ), false );
  }
}

void ImageListDialog::addURLList( const QStrList &list, const bool show )
{
  QStrListIterator it( list );
  if( it.current() )
  {
    addURL( KURL( it.current() ), show );
    while( ++it )
      addURL( KURL( it.current() ), false );
    m_imageListBox->repaint();
  }
}

void ImageListDialog::slot_select( QListBoxItem *item )
{
  if( m_imagelist.isEmpty() ) return;

  const int index = m_imageListBox->index( item );

  (void)m_imagelist.first();
  for( int i = 0; i < index; i++ )
    (void)m_imagelist.next();

  loadImage();
}

void ImageListDialog::slot_shuffle()
{
  if( m_imagelist.isEmpty() ) return;

  KRandomSequence krand( kapp->random() );
  krand.randomize( &m_imagelist );
  m_imageListBox->clear();

  (void)m_imagelist.first();
  do
  {
    m_imageListBox->insertItem( m_imagelist.current()->url.prettyURL() );
  } while( m_imagelist.next() != 0 );

  (void)m_imagelist.first();
  loadImage();
}

void ImageListDialog::slot_sort()
{
  if( m_imagelist.isEmpty() ) return;

  m_imagelist.sort();
  m_imageListBox->clear();

  (void)m_imagelist.first();
  do
  {
    m_imageListBox->insertItem( m_imagelist.current()->url.prettyURL() );
  } while( m_imagelist.next() != 0 );

  (void)m_imagelist.first();
  loadImage();
}

void ImageListDialog::slot_slideshow( bool start )
{
  if( m_imagelist.isEmpty() ) return; //no images to show
  if( !m_slideTimer )
  {
    m_slideTimer = new QTimer( this, "Slideshow Timer" );
    connect( m_slideTimer, SIGNAL( timeout() ), this, SLOT( slot_nextSlide() ) );
  }

  if( start && !m_slideTimer->isActive() )
  {
    slot_nextSlide();
    m_slideTimer->start( m_slideInterval * 1000 );
    m_slideshow->setText( i18n( "Stop &Slideshow" ) );
  }
  else if( !start && m_slideTimer->isActive() )
  {
    m_slideTimer->stop();
    m_slideshow->setText( i18n( "Start &Slideshow" ) );
  }
}

void ImageListDialog::slot_toggleSlideshow()
{
  m_slideshow->toggle();
}
    
void ImageListDialog::slot_nextSlide()
{
  //if( m_imagelist.current() == m_imagelist.getLast() )
  //{
    //if( m_loop )
      //slot_first();
    //else
      //slot_slideshow( false ); //stops the Slideshow
  //}
  //else
    slot_next();
}

void ImageListDialog::slot_pauseSlideshow()
{
  if( slideShowRunning() )
  {
    m_slideTimer->stop();
    m_paused = true;
  }
}

void ImageListDialog::slot_continueSlideshow()
{
  if( m_paused )
  {
    m_slideTimer->start( m_slideInterval * 1000 );
    m_paused = false;
  }
}

void ImageListDialog::saveOptions( KConfig *cfg ) const
{
  cfg->writeEntry( "SlideInterval", m_slideInterval );
  cfg->writeEntry( "SlideLoop", m_loop );
}

void ImageListDialog::restoreOptions( const KConfig *cfg )
{
  m_slideInterval = cfg->readNumEntry( "SlideInterval", 5 );
  m_loop = cfg->readBoolEntry( "SlideLoop", true );
}

void ImageListDialog::saveProperties( KConfig *cfg )
{
  // slideshow settings
  cfg->writeEntry( "SlideInterval", m_slideInterval );
  cfg->writeEntry( "SlideLoop", m_loop );

  // url list
  cfg->writeEntry( "ListNumUrls", m_imagelist.count() );

  int i = 0;
  if( !m_imagelist.isEmpty() )
  {
    (void)m_imagelist.first();
    do
    {
      QString tag = QString( "ListUrl%1" ).arg( i );
      cfg->writeEntry( tag, m_imagelist.current()->url.url() );
    } while( m_imagelist.next() != 0 );
  }

  // geometry
  cfg->writeEntry( "ListWinSize" , size() );
  cfg->writeEntry( "ListWinPos" , pos() );
  cfg->writeEntry( "ListVisible", isVisible() );
}

void ImageListDialog::restoreProperties( KConfig *cfg )
{
  m_slideInterval = cfg->readNumEntry( "SlideInterval", 5 );
  m_loop = cfg->readBoolEntry( "SlideLoop", false );

  int urlCount = cfg->readNumEntry( "ListNumUrls", 0 );

  for( int i = 0; i < urlCount; i++ )
  {
    // load each image
    QString tag = QString( "ListUrl%1" ).arg( i );
    if( !cfg->hasKey( tag ) )
      continue;

    QString url = cfg->readEntry( tag );
    addURL( url, (i == 0) );
  }

  // geometry
  if( cfg->hasKey(  "ListWinSize" ) )
  {
    resize( cfg->readSizeEntry( "ListWinSize") );
  }
  if( cfg->hasKey( "ListWinPos" ) )
  {
    move( cfg->readPointEntry( "ListWinPos" ) );   
  }
  if ( cfg->readBoolEntry( "ListVisible", false ) )
  {
    show();
  }           
}

void ImageListDialog::slot_save()
{
  KURL url = KFileDialog::getSaveURL( ":save_list", QString::null, this );

  if( url.isEmpty() ) return;
  QString tempfile;

  if( url.isLocalFile() )
    tempfile = url.path();
  else
  {
    KTempFile ktempf;
    tempfile = ktempf.name();
  }
  
  //save file right here
  QFile file( tempfile );
  if( file.open( IO_WriteOnly ) )
  {
    int oldposition = m_imagelist.at();
    QTextStream t( &file );

    t << "[KView Image List]" << endl;
    (void)m_imagelist.first();
    do
    {
      t << m_imagelist.current()->url.url() << endl;
    } while( m_imagelist.next() != 0 );

    file.close();

    (void)m_imagelist.first();
    for( int i = 0; i < oldposition; i++ ) 
      (void)m_imagelist.next();

    KIO::NetAccess::upload( tempfile, url );
    KIO::NetAccess::removeTempFile( tempfile );
  }
}

void ImageListDialog::slot_load()
{
  KURL url = KFileDialog::getOpenURL( ":load_list", QString::null, this );

  if( url.isEmpty() ) return;
  QString tempfile;

  if( ! KIO::NetAccess::download( url, tempfile ) )
  {
    KMessageBox::error( this, i18n( "Could not load\n%1" ).arg( url.prettyURL() ) );
    return;
  }
  
  QFile file( tempfile );
  if( file.open( IO_ReadOnly ) )
  {
    QTextStream t( &file );
    if( t.readLine() == "[KView Image List]" )
    {
      //clear old image list
      (void) m_imagelist.first();
      while( ! m_imagelist.isEmpty() )
      {
        KIO::NetAccess::removeTempFile( m_imagelist.current()->filename );
        m_imagelist.remove();
      }
      m_imageListBox->clear();
      
      QStrList list;
      while( !t.eof() )
      {
        list.append( t.readLine().ascii() );
      }

      addURLList( list ); 
    }
    else
    {
      KMessageBox::error( this, i18n( "Wrong format\n%1" ).arg( url.prettyURL() ) );
    }
      
    file.close();
  }
  KIO::NetAccess::removeTempFile( tempfile );
}
    
#include "imagelistdialog.moc"
