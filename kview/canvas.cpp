/*
* canvas.cpp -- Implementation of class KImageCanvas.
* Author:       Sirtaj Singh Kang
* Version:      $Id: canvas.cpp 110000 2001-08-10 15:01:12Z mkretz $
* Generated:    Thu Oct  9 09:03:54 EST 1997
*/

#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include <qcolor.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <assert.h>
#include <qlabel.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpen.h>
#include <qmovie.h>
#include <qfile.h>
#include <qbitmap.h>

#include <kdebug.h>
#include <dcopclient.h>
#include <kapp.h>
#include <kstddirs.h>
#include <ktempfile.h>

#include <X11/Xlib.h>

#include "canvas.h"

static QString loadStdin();
static void maxpect( QWidget *dest, QPixmap *image );

KImageCanvas::KImageCanvas( QWidget *parent )
  : QScrollView( parent ),
  _status( OK ),
  _file( "" ),
  _client( 0 ),
  _orig( 0 ),
  m_centered( false ),
  m_depth( 0 )
{
  _client = new KVImageHolder( viewport() );
  addChild( _client, 0, 0 );

  connect(_client, SIGNAL( contextPress( const QPoint& ) ), SIGNAL( contextPress( const QPoint& ) ) );
  connect(_client, SIGNAL( selected( bool ) ), SIGNAL( selected( bool ) ) );

  _client->hide();
}

KImageCanvas::~KImageCanvas()
{
  if( _orig )
    delete _orig;
  _orig = 0;
}

void KImageCanvas::setBgColor( const QColor & color )
{
  viewport()->setBackgroundColor( color );
}

const QColor & KImageCanvas::bgColor() const
{
  return viewport()->backgroundColor();
}

int KImageCanvas::pixmapDepth() const
{
  return m_depth;
}

QSize KImageCanvas::originalSize() const
{
  if( isEmpty() )
    return QSize( 0, 0 );

  return m_originalsize;
  /*if( _orig )
    return _orig->size();
  else
    return _client->imagePix()->size();*/
}

void KImageCanvas::slot_setCentered( bool centered )
{
	m_centered = centered;
	if( m_centered )
	{
		setResizePolicy( Manual );
		resizeContents( QMAX( width(), _client->width() ), QMAX( height(), _client->height() ) );
		moveChild( _client, QMAX( 0, ( viewport()->width() - _client->width() ) / 2), QMAX( 0, ( viewport()->height() - _client->height() ) / 2 ));
		center( _client->width() / 2, _client->height() / 2 );
	}
	else
	{
		setResizePolicy( AutoOne );
		moveChild( _client, 0, 0 );
	}
}

KImageCanvas::Error KImageCanvas::status() const
{
  return _status;
}

int KImageCanvas::load( const QString & file, const QString & URL, bool max )
{
  if( file == 0 ) {
    setStatus( BadPath );
    return 0;
  }

  QString realfile = file;
  bool std = false;

  if( realfile == "-" ) {
    // stdin
    std = true;
    realfile = loadStdin();

    if( realfile.isEmpty() ) {
      setStatus( BadPath );
      return 0;
    }
  }

  static int allocContext = 0;
  if( allocContext )  {
    QColor::destroyAllocContext( allocContext );
  }

  allocContext = QColor::enterAllocContext();

  QApplication::setOverrideCursor( WaitCursor );

  QImage newImage;
  bool loadOK = newImage.load( realfile );

  if( !loadOK && realfile.contains( ".tga", false ) ) {
    loadOK = newImage.load( realfile, "TGA" );
  }
  m_depth = newImage.depth();
  QPixmap newPixmap;
  newPixmap.convertFromImage( newImage, ColorOnly );

  QColor::leaveAllocContext();

  if( std ) {
    unlink( QFile::encodeName( realfile ) );
  }

  if( loadOK ) {
    // delete old buffered image
    forgetOriginal();

    //put the new image into the _client
    m_originalsize = newPixmap.size();
    if ( max ) {
      _orig = new QPixmap( newPixmap );
      maxpect( this, &newPixmap );
    }

	_client->hide();
    _client->setImagePix( newPixmap );

    //and readd the contents again
    if( m_centered )
    {
	  resizeContents( QMAX( width(), _client->width() ), QMAX( height(), _client->height() ) );
      int x = QMAX( 0, ( viewport()->width() - _client->width() ) / 2 );
      int y = QMAX( 0, ( viewport()->height() - _client->height() ) / 2 );
      moveChild( _client, x, y );
	  center( _client->width() / 2, _client->height() / 2 );
    }
	_client->show();

    updateScrollBars();

    _file = ( !URL.isNull() ? URL : realfile );
    setStatus( OK );
    setCaption( _file );
    QApplication::restoreOverrideCursor();
    return -1;
  }
  else {
    kdWarning() << "Couldn't open " << realfile << endl;
    setStatus( BadPath );
  }

  QApplication::restoreOverrideCursor();
  return 0;
}

bool KImageCanvas::save( const QString &filename, QString &format )
{
  if( format == QString::null )
    format = "BMP";

  //if( format == "JPEG" )
  //{
    //TODO: Ask for quality
  //}

  return _client->imagePix()->save( filename, format.ascii() );
}

void KImageCanvas::reset()
{
  if( _orig == 0 ) {
    return;
  }

  _client->setImagePix( *_orig );
  emit imageSizeChanged();
}

void KImageCanvas::forgetOriginal()
{
  delete _orig; // delete old buffered image
  _orig = 0;
}

void KImageCanvas::clear()
{
  _client->clear();
}

bool KImageCanvas::isEmpty() const
{
  return ! _client->imagePix();
}

void KImageCanvas::transformImage( const QWMatrix& mat )
{
  const QPixmap *image = transPixmap();

  if( image == 0 ) {
    return;
  }

  QApplication::setOverrideCursor( waitCursor );
  QPixmap newimage = _orig->xForm( mat );

  _client->clearSelection();
  _client->setImagePix( newimage );

  QApplication::restoreOverrideCursor();
}

void KImageCanvas::slot_tileToDesktop() const
{
  setDesktopBackground( Tiled );
}

void KImageCanvas::slot_maxToDesktop() const
{
  setDesktopBackground( Scaled );
}

void KImageCanvas::slot_maxpectToDesktop() const
{
  setDesktopBackground( CentredMaxpect );
}

void KImageCanvas::setDesktopBackground( WallpaperMode mode ) const
{
  QApplication::setOverrideCursor( waitCursor );

  if( _client->imagePix() == 0 )
    return;
  
  QPixmap image = *( _client->imagePix() );

  QString filename = kapp->dirs()->saveLocation( "data", "kview/" ) + "wallpaper.jpg";
  image.save( filename, "JPEG" );

  DCOPClient *client = kapp->dcopClient();
  if( ! client->isAttached() )
    client->attach();

  int screen_number = 0;
  if ( qt_xdisplay() )
    screen_number = DefaultScreen( qt_xdisplay() );
  QCString appname;
  if ( screen_number == 0 )
    appname = "kdesktop";
  else
    appname.sprintf( "kdesktop-screen-%d", screen_number );

  QByteArray data;
  QDataStream arg( data, IO_WriteOnly );
  arg << filename;
  arg << (int)mode;

  client->send( appname, "KBackgroundIface", "setWallpaper(QString,int)", data );

  QApplication::restoreOverrideCursor();
}

static void maxpect( QWidget *dest, QPixmap *image )
{
  QApplication::setOverrideCursor( WaitCursor );
  double dh = double( dest->height() ) / double( image->height() );
  double dw = double( dest->width() ) / double( image->width() );
  double d = ( dh < dw ? dh : dw );
  
  image->convertFromImage(image->convertToImage().smoothScale(
      int( d*image->width() ), int( d*image->height() ) ) );
  QApplication::restoreOverrideCursor();
}

void KImageCanvas::resizeEvent( QResizeEvent *ev )
{
  QScrollView::resizeEvent( ev );

  emit imageSizeChanged();
}

QImage KImageCanvas::getImage() const
{
  return _client->imagePix()->convertToImage();
}

void KImageCanvas::slot_setImage( const QImage& image )
{
  transPixmap();

  QPixmap pixmap;
  pixmap.convertFromImage( image );

  _client->setImagePix( pixmap );

  emit imageSizeChanged();
}

QPixmap *KImageCanvas::transPixmap()
{
  QPixmap *client = _client->imagePix();

  if( client == 0 ) {
    return 0;
  }

  if( _orig == 0 ) {
    _orig = new QPixmap( *client );
  }

  return client;
}

void KImageCanvas::slot_cropImage()
{
  QRect select = _client->selected();

  if ( select.isNull() ) {
    return;
  }

  transPixmap();

  QPixmap *oldpix = _client->imagePix();

  QPixmap newpix( select.width(), select.height() );
  
  bitBlt( &newpix, 0, 0, oldpix, select.left(),
    select.top(), select.width(), select.height(), CopyROP );

  _client->setImagePix( newpix );
  emit imageSizeChanged();
}


#define BUF_SIZE 1024
/**
* Read standard input into a file.
* @return filename
*/
static QString loadStdin()
{
  if( feof( stdin ) ) {
    return QString::null;
  }

  KTempFile tempfile;

  char buffer[ BUF_SIZE ];
  FILE *o = tempfile.fstream();

  if( o == 0 ) {
    return QString::null;
  }

  while( !feof( stdin ) ) {
    size_t bytes = fread( buffer, sizeof(char), BUF_SIZE, stdin );

    fwrite( buffer, sizeof( char ), bytes, o );
  }
  tempfile.close();

  return tempfile.name();
}


void KImageCanvas::slot_maxToWin()
{
  transPixmap();

  if( _client->imagePix() == 0 )
    return;
  
  QPixmap image = *_orig;

  QApplication::setOverrideCursor( WaitCursor );
  image.convertFromImage(image.convertToImage().smoothScale(
      width(),height()));
  _client->setImagePix( image );
  QApplication::restoreOverrideCursor();
  
  emit imageSizeChanged();
}

void KImageCanvas::slot_maxpectToWin()
{
  transPixmap();

  if( _client->imagePix() == 0 )
    return;
  
  QPixmap *image = _client->imagePix();

        maxpect( this, image );

  _client->setImagePix( *image );
  emit imageSizeChanged();
}

void KImageCanvas::viewportMousePressEvent( QMouseEvent *ev )
{
  if ( ev->button() == RightButton )
    emit contextPress( mapToGlobal( ev->pos() ) );
}

void KImageCanvas::copyImage( QPaintDevice *dest ) const
{
  // only print if there is an image
  if( isEmpty() )
    return;
  QPainter painter( dest );
  painter.drawPixmap( 0, 0, *(_client->imagePix()) );
}

/*
******************************
* KVImageHolder implementation
*/

KVImageHolder::KVImageHolder( QWidget *parent )
    : QLabel( parent ),
    _selected( false ),
    m_empty( true ),
    _painter( new QPainter ),
    _pen( new QPen( QColor( 255, 255, 255 ), 0, DashLine ) )
{
  assert( _painter != 0 );
  setBackgroundMode(QWidget::NoBackground);
}

KVImageHolder::~KVImageHolder()
{
  delete _painter;
  delete _pen;
}

void KVImageHolder::mousePressEvent( QMouseEvent *ev )
{
    if ( ev->button() == RightButton ) {
      emit contextPress( mapToGlobal( ev->pos() ) );
      return;
    }

    if( pixmap() == 0 ) {
      return;
    }

    if( _selected ) {
      // remove old rubberband
      eraseSelect();
      _selected = false;
            emit selected( false );
    }

    _selection.setLeft( ev->x() );
    _selection.setTop( ev->y() );
    emit selected( true );
}

void KVImageHolder::mouseMoveEvent( QMouseEvent *ev )
{
  // currently called only on drag,
  // so assume a selection has been started
  bool erase = _selected;

  if( !_selected ) {
    _selected = true;
  }

  int r = (ev->x() < width()) ? ev->x() : width() - 1;
  int b = (ev->y() < height()) ? ev->y() : height() - 1;

  if( r != _selection.right() || b != _selection.bottom() ) {
    if ( erase )
      eraseSelect();
    
    _selection.setRight( r );
    _selection.setBottom( b );

    drawSelect();
  }
}

void KVImageHolder::drawSelect()
{
  _painter->begin( this );
  _painter->setRasterOp( XorROP );
  _painter->setPen( *_pen );
  _painter->drawRect( _selection );
  _painter->end();
}

void KVImageHolder::eraseSelect()
{
  QRect r = _selection.normalize();

  bitBlt( this, r.left(), r.top(), pixmap(), r.left(), r.top(),
      r.width(), r.height(), CopyROP );
}

void KVImageHolder::paintEvent( QPaintEvent *ev )
{
  QLabel::paintEvent( ev );

  if( _selected ) {
    drawSelect();
  }
}

QRect KVImageHolder::selected() const
{
  if( !_selected ) {
    return QRect();
  }

  return _selection.normalize();
}

void KVImageHolder::setImagePix( QPixmap pix )
{
  clearSelection();

  const QBitmap nullBitmap;
  pix.setMask( nullBitmap ); //don't show transparency

  setPixmap( pix );
  m_empty = false;
  resize( pix.size() );
}

void KVImageHolder::setImageMovie( const QMovie &movie )
{
  clearSelection();
  setMovie( movie );
  m_empty = false;
}

QPixmap *KVImageHolder::imagePix()
{
  if( m_empty )
    return 0;

  if ( movie() )
    return &((QPixmap&)(movie()->framePixmap()));
    
  return pixmap();
}

void KVImageHolder::clear()
{
  setBackgroundColor( QColor( 0, 0, 0 ) );
  resize( 0, 0 );
  m_empty = true;
}

#include "canvas.moc"

