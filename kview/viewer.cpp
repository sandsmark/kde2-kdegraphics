/*
 * viewer.cpp -- Implementation of class KImageViewer.
 * Author:       Sirtaj Singh Kang
 * Version:      $Id: viewer.cpp 110000 2001-08-10 15:01:12Z mkretz $
 * Generated:    Wed Oct 15 11:37:16 EST 1997
 */

#include <assert.h>
#include <unistd.h>

#include <qpopupmenu.h>
#include <qstrlist.h>
#include <qaccel.h>
#include <qtimer.h>
#include <qevent.h>
#include <qdragobject.h>
#include <qcolor.h>
#include <qfileinfo.h>
#include <qdatetime.h>

#include <kapp.h>
#include <kmainwindow.h>
#include <kmenubar.h>
#include <ktoolbar.h>
#include <kprinter.h>
#include <kfiledialog.h>
#include <kaccel.h>
#include <kkeydialog.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kconfig.h>
#include <kimageio.h>
#include <kio/netaccess.h>
#include <kwin.h>
#include <kdebug.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kedittoolbar.h>
#include <ktempfile.h>
#include <kwinmodule.h> //to get the workarea Rect

#include "viewer.h"
#include "canvas.h"
#include "version.h"
#include "knumdialog.h"
#include "filter.h"
#include "kfilteraction.h"
#include "filtlist.h"
#include "imagelistdialog.h"
#include "kviewconfdialog.h"
#include "infowin.h"

enum {
  ID_FILENAME,
  ID_PROGRESS,
  ID_STATUS,
  ID_SIZE
};

const int MessageDelay = 3000;

KImageViewer::KImageViewer( KFilterList* filters )
  : KMainWindow( 0 ),
  _imageLoaded( false ),
  m_bFullScreen( false ),
  m_recent( 0 ),
  m_filterMenu( 0 ),

  _pctBuffer( new QString ),
  _lastPct( - 1 ),
  _msgTimer( 0 ),
  _imageList( new ImageListDialog ),
  //_zoomFactor( 100 ),

  m_resizeMode ( ResizeWindow | ResizeImage ),
  m_kwinmodule( new KWinModule( this ) ),
  m_imageinfo( 0 ),
  m_toolbarorientationchanged( false )
{
  // Image canvas
  _canvas = new KImageCanvas( this );

  connect(_canvas, SIGNAL( contextPress(const QPoint&) ),
      this, SLOT ( slot_contextPress(const QPoint&) ));

  assert( _canvas );
  setCentralWidget( _canvas );
  _canvas->setFrameStyle( QFrame::NoFrame );

  m_accel = new KAccel(this);

  // Make it a reasonable size
  resize( 250, 150 );

  // Caption
  setCaption( i18n( "no image loaded" ) );

  // make a StatusBar
  statusBar()->insertItem( i18n( "no image loaded" ), ID_FILENAME, 1 );
  statusBar()->setItemAlignment( ID_FILENAME, AlignLeft | AlignVCenter );
  statusBar()->insertItem( i18n( " Ready " ), ID_STATUS, 0, true );
  //statusBar()->insertItem( "                 ", ID_PROGRESS, 0, true );
  statusBar()->insertItem( " 0x0 ", ID_SIZE, 0, true );

  // list dialog
  connect( _imageList, SIGNAL( pleaseLoad( const QString&, const KURL& ) ),
      this, SLOT( slot_loadFile( const QString&, const KURL& ) ) );

  setupActions( filters );

  statusBar()->hide();
  toolBar()->hide();

  setAcceptDrops( true );
  restoreOptions();

  //resize the window if the ToolBar moves
  connect( this, SIGNAL( toolBarPositionChanged( QToolBar * ) ), SLOT( slot_toolBarMoved() ) );
  connect( toolBar(), SIGNAL( orientationChanged( Orientation ) ), SLOT( slot_toolBarOrientationChanged() ) );
}

KImageViewer::~KImageViewer()
{
  m_recent->saveEntries( kapp->config() );

  kapp->config()->sync();

  delete _canvas; _canvas = 0;

  delete _pctBuffer;
  delete _imageList;
}

void KImageViewer::slot_load()
{
  KURL::List urls = KFileDialog::getOpenURLs( ":load_image", KImageIO::pattern( KImageIO::Reading ), this );

  if( urls.isEmpty() )
    return;

  for( KURL::List::Iterator it = urls.begin(); it != urls.end(); it++ )
  {
    _imageList->addURL( *it, false );
    m_recent->addURL( *it );
  }
  _imageList->slot_last();
  //firstImage();
}

void KImageViewer::slot_firstImage()
{
  _imageList->slot_first(); 
}

void KImageViewer::slot_openRecent( const KURL& url )
{
  if( url.isEmpty() ) return;

  _imageList->addURL( url );
}

void KImageViewer::slot_fileClose()
{
  if( _imageList->removeCurrent() )
  {
    /* this was the last image in the list */

    _canvas->clear();
    menuEntriesEnabled( false );
    _imageLoaded = false;
    setCaption( i18n( "no image loaded" ) );
    statusBar()->changeItem( i18n( "no image loaded" ), ID_FILENAME );
    statusBar()->changeItem( " 0x0 ", ID_SIZE );
    m_filename = "";
    m_url = KURL( "" );
    m_format = "PNG";
  }

  m_editCrop->setEnabled( false );
}

void KImageViewer::slot_quitApp()
{
  kapp->quit();
}

void KImageViewer::menuEntriesEnabled( bool b )
{
  m_rotateMenu->setEnabled( b );
  m_flipMenu->setEnabled( b );
  m_desktopMenu->setEnabled( b );
  m_zoomMenu->setEnabled( b );
  m_zoom50->setEnabled( b );
  m_zoom100->setEnabled( b );
  m_zoom200->setEnabled( b );
  m_zoomMaxpect->setEnabled( b );
  m_zoomMax->setEnabled( b );
  m_fileSave->setEnabled( b );
  m_fileSaveAs->setEnabled( b );
  m_fileClose->setEnabled( b );
  m_filePrint->setEnabled( b );
  m_editReset->setEnabled( b );
  m_viewZoom->setEnabled( b );
  m_viewZoomIn->setEnabled( b );
  m_viewZoomOut->setEnabled( b );
  m_viewDouble->setEnabled( b );
  m_viewHalf->setEnabled( b );
  m_rotate90->setEnabled( b );
  m_rotate180->setEnabled( b );
  m_rotate270->setEnabled( b );
  m_flipV->setEnabled( b );
  m_flipH->setEnabled( b );
  m_desktopTile->setEnabled( b );
  m_desktopMax->setEnabled( b );
  m_desktopMaxpect->setEnabled( b );
  m_imagePrev->setEnabled( b );
  m_imageNext->setEnabled( b );
  m_imageFirst->setEnabled( b );
  m_imageLast->setEnabled( b );
  m_imageSlide->setEnabled( b );
  m_imageInfo->setEnabled( b );
  m_filterMenu->setEnabled( b );
}

void KImageViewer::dragEnterEvent(QDragEnterEvent* e)
{
  e->accept(
      QUriDrag::canDecode( e ) |
      QImageDrag::canDecode( e )
           );
}

void KImageViewer::dropEvent(QDropEvent *e)
{
  QStrList uri;
  QImage image;
  if( QUriDrag::decode(e, uri) )
  {
    _imageList->addURLList( uri );
  }
  else if( QImageDrag::decode( e, image ) )
  {
    addImage( image );
  }
}

void KImageViewer::addImage( const QImage &image )
{
  KTempFile tempfile;
  QString filename = tempfile.name();
  tempfile.close();

  if( ! image.save( filename, "PNG" ) ) return;

  _imageList->addURL( KURL( filename ) );
}

void KImageViewer::slot_saveAs()
{
  KURL url = KFileDialog::getSaveURL( ":save_image", KImageIO::pattern( KImageIO::Writing ), this );

  if ( url.isEmpty())
    return;

  bool stat = false;

  if( url.isMalformed() )
  {
    slot_message( i18n("Malformed URL.") );
  }

  QString tmpfile;
  if( url.isLocalFile() )
    tmpfile = url.path();
  else
  {
    KTempFile ktempf;
    tmpfile = ktempf.name();
  }

  stat = saveFile( tmpfile, KImageIO::type( url.path() ), url );

  if( stat == false )
  {
    slot_message( i18n( "the file wasn't saved" ) );
    return;
  }

  slot_message( i18n("%1: written").arg(url.prettyURL()) );
  m_recent->addURL( url );
}

void KImageViewer::slot_save()
{
  if( ! saveFile( m_filename, m_format, m_url ) )
  {
    slot_message( i18n( "the file wasn't saved" ) );
    return;
  }

  slot_message( i18n("%1: written").arg(m_url.prettyURL()) );
}

bool KImageViewer::saveFile( const QString &filename, QString format, const KURL &url )
{
  if( !_canvas->save( filename, format ) )
    return false;

  m_filename = filename;
  m_format = format;


  if( !url.isEmpty() )
  {
    if( !KIO::NetAccess::upload( filename, url ) )
      return false;

    m_url = url;
  }
  else
  {
    m_url = m_filename;
  }

  _imageList->setFilename( m_filename );
  _imageList->setURL( m_url );

  setCaption( m_url.prettyURL() );
  statusBar()->changeItem( m_filename, ID_FILENAME );

  _canvas->forgetOriginal();
  return true;
}

void KImageViewer::slot_zoomIn10()
{
  zoom( 1.1 );
}

void KImageViewer::slot_zoomOut10()
{
  zoom( 0.9 );
}

void KImageViewer::slot_zoomIn200()
{
  zoom( 2.0 );
}

void KImageViewer::slot_zoomOut50()
{
  zoom( 0.5 );
}

void KImageViewer::slot_zoomCustom()
{
  KNumDialog num( this );
  double zoomFactor = _mat.m11() * 100;

  if( !num.getNum( zoomFactor, i18n("Enter Zoom factor (100 = 1x):") ) )
    return;

  if( zoomFactor <= 0 )
  {
    slot_message( i18n( "Illegal zoom factor" ) );
    return;
  }

  double val = zoomFactor / 100;
  _mat.reset();
  zoom( val );
}

void KImageViewer::zoom( double factor )
{
  if( m_resizeMode & FastScale )
  {
    _mat.scale( factor, factor );
    _canvas->transformImage( _mat );
  }
  else
  {
    int width  = (int)( _canvas->contentsWidth()  * factor );
    int height = (int)( _canvas->contentsHeight() * factor );
    sizeCorrection( width, height );
    resize( width, height );
    _canvas->slot_maxToWin();
  }
  rzWinToImg();
}

void KImageViewer::slot_rotate90()
{
  _mat.rotate( 90 );
  _canvas->transformImage( _mat );
  rzWinToImg();
}

void KImageViewer::slot_rotate180()
{
  _mat.rotate( 180 );
  _canvas->transformImage( _mat );
  rzWinToImg();
}

void KImageViewer::slot_rotate270()
{
  _mat.rotate( 270 );
  _canvas->transformImage( _mat );
  rzWinToImg();
}

void KImageViewer::slot_flipVertical()
{
  QWMatrix m( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F );

  _mat *= m;

  _canvas->transformImage( _mat );
  rzWinToImg();
}

void KImageViewer::slot_flipHorizontal()
{
  QWMatrix m( -1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F );

  _mat *= m;

  _canvas->transformImage( _mat );
  rzWinToImg();
}

void KImageViewer::slot_tile()
{
  _canvas->slot_tileToDesktop();
}

void KImageViewer::slot_max()
{
  _canvas->slot_maxToDesktop();
}

void KImageViewer::slot_maxpect()
{
  _canvas->slot_maxpectToDesktop();
}

void KImageViewer::slot_appendURL( const KURL &url, bool show )
{
  _imageList->addURL( url, show );
}

void KImageViewer::slot_invokeFilter( KImageFilter *f )
{
  assert( f != 0 );

  QObject::disconnect( f, SIGNAL( changed( const QImage& ) ), 0, 0 );
  connect( f, SIGNAL( changed( const QImage& ) ), _canvas, SLOT( slot_setImage( const QImage& ) ) );

  f->invoke( _canvas->getImage() );
}

void KImageViewer::connectFilters( KFilterList *filters )
{
  // connect all filters
  for( int i = 0; i < filters->count(); i++ )
  {
    KImageFilter *filter = filters->filter( i );

    connect( filter, SIGNAL( changed( const QImage& ) ),
        _canvas, SLOT( slot_setImage( const QImage& ) ) );
    //connect( filter, SIGNAL( progress( int) ),
        //SLOT( slot_setProgress( int ) ) );
    connect( filter, SIGNAL( status( const QString& ) ),
        SLOT( slot_setStatus( const QString& ) ) );
    connect( filter, SIGNAL( message( const QString& ) ),
        SLOT( slot_message( const QString& ) ) );
    connect( filter, SIGNAL( selected( KImageFilter* ) ),
        SLOT( slot_invokeFilter( KImageFilter* ) ) );
  }
}

void KImageViewer::slot_setStatus( const QString & status )
{
  if ( status.isEmpty() )
    statusBar()->changeItem( i18n( " Ready " ), ID_STATUS );
  else
    statusBar()->changeItem( status, ID_STATUS );
}

void KImageViewer::slot_loadFile( const QString & file, const KURL &_url )
{
  KURL url(_url);
  if( url.isEmpty() )
  {
    url = file;
  }

  slot_setStatus( i18n( "Loading..." ) );

  // load the image
  _canvas->load( file, 0, m_bFullScreen && ( m_resizeMode & ResizeImage ) );

  // update state
  slot_setStatus( 0 );

  if( _canvas->status() != KImageCanvas::OK )
  {
    slot_message( i18n( "Could not load %1" ).arg( url.prettyURL() ) );
    _imageList->removeCurrent();
  }
  else
  {
    // reset matrix
    _mat.reset();

    // make all the image infos
    if( !m_imageinfo )
      m_imageinfo = new InfoWin();
    QFileInfo fileinfo( file );
    QSize size = _canvas->originalSize();
    QString dimensions = " " + QString::number( size.width() ) + "x" + QString::number( size.height() ) + " ";
    statusBar()->changeItem( dimensions, ID_SIZE );
    m_imageinfo->setDimensions( size.width(), size.height() );
    m_imageinfo->setSize( fileinfo.size() );
    m_imageinfo->setIsWritable( fileinfo.isWritable() );
    m_imageinfo->setLastModified( fileinfo.lastModified().toString() );
    m_imageinfo->setDepth( _canvas->pixmapDepth() );

    if( ! m_bFullScreen )
    {
      if( m_resizeMode == ResizeImage )
	_canvas->slot_maxToWin();
      else if( m_resizeMode == ResizeWindow )
	rzWinToImg();
      else if( m_resizeMode & ( ResizeImage | ResizeWindow ) )
      {
	slot_zoom100();
	if( ( _canvas->contentsWidth() > _canvas->visibleWidth() ) || ( _canvas->contentsHeight() > _canvas->visibleHeight() ) )
	  slot_zoomMaxpect();
      }
    }

    // set caption
    setCaption( url.prettyURL() );
    statusBar()->changeItem( file, ID_FILENAME );

    m_filename = file;
    m_format = KImageIO::type( url.path() );
    m_url = url;

    // enable Menu entries
    if( !_imageLoaded )
    {
      menuEntriesEnabled( true );
      _imageLoaded = true;
    }
  }
}

/*void KImageViewer::setProgress( int pct )
{
  if( pct == _lastPct )
    return;

  QString buf;
  if( pct > 0 ) {
    _pctBuffer->setNum( pct );
    *_pctBuffer += '%';
    buf = *_pctBuffer;
  }

  _lastPct = pct;

  statusBar()->changeItem( buf, ID_PROGRESS );
}*/

void KImageViewer::slot_message( const QString &message )
{
  statusBar()->message( message, MessageDelay );
}

void KImageViewer::slot_toggleImageList()
{
  if ( _imageList->isVisible() ) {
    _imageList->hide();
  }
  else {
    _imageList->show();
  }
}


void KImageViewer::slot_contextPress(const QPoint& p)
{
  QPopupMenu *pop = new QPopupMenu( this, "popupmenu" );

  m_settingsShowMenuBar->plug( pop );
  pop->insertSeparator();
  m_imageList->plug( pop );
  m_imagePrev->plug( pop );
  m_imageNext->plug( pop );
  m_imageSlide->plug( pop );
  pop->insertSeparator();
  m_viewFullScreen->plug( pop );

  pop->popup( p );
}

void KImageViewer::slot_fullScreen()
{
  m_bFullScreen = !m_bFullScreen;
  if( m_bFullScreen )
  {
    _posSave = pos();
    _sizeSave = size();

    // save the looks of the window so we can restore it later
    saveMainWindowSettings( KGlobal::config(), "MainWindow" );

    // Preserve caption, reparent calls setCaption (!)
    showFullScreen();
    setCaption( m_filename );

    // Qt bug (see below)
    setAcceptDrops( FALSE );
    topData()->dnd = 0;
    setAcceptDrops( TRUE );

    // change to full
    toolBar()->hide();
    m_settingsShowToolBar->setChecked( false );
    statusBar()->hide();
    m_settingsShowStatusBar->setChecked( false );
    menuBar()->hide();
    m_settingsShowMenuBar->setChecked( false );

    _canvas->slot_setCentered( true );

    m_viewFullScreen->setText( i18n( "Stop Fullscreen Mode" ) );
    m_viewFullScreen->setIcon( "window_nofullscreen" );
  }
  else
  {
    move( _posSave );
    resize( _sizeSave );

    showNormal();  // (calls setCaption, i.e. the one in this class!)
    applyMainWindowSettings( KGlobal::config(), "MainWindow" );
    setCaption( m_filename );

    // Qt bug, the flags aren't restored.
    setWFlags( WType_TopLevel | WDestructiveClose );
    // Other Qt bug
    setAcceptDrops( FALSE );
    topData()->dnd = 0;
    setAcceptDrops( TRUE );

    m_settingsShowToolBar->setChecked( !toolBar()->isHidden() );
    m_settingsShowStatusBar->setChecked( !statusBar()->isHidden() );
    m_settingsShowMenuBar->setChecked( !menuBar()->isHidden() );

    _canvas->slot_setCentered( false );

    m_viewFullScreen->setText( i18n( "Fullscreen Mode" ) );
    m_viewFullScreen->setIcon( "window_fullscreen" );

    kapp->setTopWidget( this ); //restores the icon
    
    rzWinToImg();
  }
}

void KImageViewer::slot_reset()
{
  _canvas->reset();
  _mat.reset();
  rzWinToImg();
}

void KImageViewer::saveProperties( KConfig *cfg )
{
  cfg->writeEntry( "ViewerFullScreen", m_bFullScreen );
  if( m_bFullScreen )
  {
    cfg->writeEntry( "ViewerPos", _posSave );
    cfg->writeEntry( "ViewerSize", _sizeSave );
  }
  else
  {
    cfg->writeEntry( "ViewerPos", pos() );
    cfg->writeEntry( "ViewerSize", size() );
  }

  _imageList->saveProperties( cfg );
}

void KImageViewer::readProperties( KConfig *cfg )
{
  m_bFullScreen = cfg->readBoolEntry( "ViewerFullScreen" );

  if( m_bFullScreen ) {
    slot_fullScreen();
    _posSave = cfg->readPointEntry( "ViewerPos" );
    _sizeSave = cfg->readSizeEntry( "ViewerSize" );
  }
  else {
    move( cfg->readPointEntry( "ViewerPos" ) );
    resize( cfg->readSizeEntry( "ViewerSize" ) );
  }

  _imageList->restoreProperties( cfg );
}

void KImageViewer::slot_saveOptions()
{
  saveMainWindowSettings( kapp->config(), "MainWindow" );
  kapp->config()->sync();
}

void KImageViewer::restoreOptions()
{
  KConfig *cfg = kapp->config();
  {
    KConfigGroupSaver save( cfg, "kview" );

    m_resizeMode = cfg->readNumEntry( "LoadMode", 3 );
  }

  if( cfg->hasGroup( "MainWindow" ) )
    applyMainWindowSettings( cfg, "MainWindow" );
  m_settingsShowToolBar->setChecked( !toolBar()->isHidden() );
  m_settingsShowStatusBar->setChecked( !statusBar()->isHidden() );
  m_settingsShowMenuBar->setChecked( !menuBar()->isHidden() );

  int r = cfg->readNumEntry( "BGColorRed", 0 );
  int g = cfg->readNumEntry( "BGColorGreen", 0 );
  int b = cfg->readNumEntry( "BGColorBlue", 0 );
  _canvas->setBgColor( QColor( r, g, b ) );

  _imageList->restoreOptions( cfg );
  m_recent->loadEntries( cfg );
}

void KImageViewer::saveConfiguration()
{
  KConfig *cfg = kapp->config();
  {
    KConfigGroupSaver save( cfg, "kview" );
    QString lmode;

    cfg->writeEntry( "LoadMode", m_resizeMode );
  }

  _imageList->saveOptions( cfg );
  cfg->writeEntry( "BGColorRed", _canvas->bgColor().red() );
  cfg->writeEntry( "BGColorGreen", _canvas->bgColor().green() );
  cfg->writeEntry( "BGColorBlue", _canvas->bgColor().blue() );

  cfg->sync();
}

void KImageViewer::slot_printImage()
{
  KPrinter printer;

  // get settings
  if( printer.setup(this) == false )
    return;

  // print

  slot_setStatus( i18n( "Printing..." ) );
  QApplication::setOverrideCursor( WaitCursor );
  _canvas->copyImage( &printer );
  printer.newPage();
  QApplication::restoreOverrideCursor();
  slot_setStatus( 0 );
}

void KImageViewer::slot_newViewer()
{
  emit wantNewViewer();
}

void KImageViewer::slot_closeViewer()
{
  emit wantToDie( this );
}

void KImageViewer::closeEvent( QCloseEvent * )
{
  slot_closeViewer();
}

void KImageViewer::slot_cut()
{
  // TODO: stub
}

void KImageViewer::slot_copy()
{
  // TODO: stub
}

void KImageViewer::slot_paste()
{
  // TODO: stub
}

void KImageViewer::slot_confAccels()
{
  KKeyDialog::configureKeys( actionCollection(), xmlFile() );
}

void KImageViewer::slot_prefs()
{
  KViewConfDialog dlg( this );
  dlg.setLoop( _imageList->loop() );
  dlg.setInterval( _imageList->interval() );
  dlg.setResize( m_resizeMode );
  dlg.setColor( _canvas->bgColor() );

  if( dlg.exec() )
  {
    _imageList->setLoop( dlg.loop() );
    _imageList->setInterval( dlg.interval() );
    m_resizeMode = dlg.resize();
    _canvas->setBgColor( dlg.color() );
    saveConfiguration();
  }
}

void KImageViewer::resizeEvent( QResizeEvent * )
{
}

void KImageViewer::keyPressEvent( QKeyEvent *ev )
{
  switch ( ev->key() )
  {
    case Key_Down:
      ev->accept();
      _canvas->slot_lineDown();
      break;
    case Key_Up:
      ev->accept();
      _canvas->slot_lineUp();
      break;
    case Key_Left:
      ev->accept();
      _canvas->slot_lineLeft();
      break;
    case Key_Right:
      ev->accept();
      _canvas->slot_lineRight();
      break;
    default:
      ev->ignore();
      break;
  }
}

void KImageViewer::slot_toolBarMoved()
{
  if( m_resizeMode == ResizeImage )
    _canvas->slot_maxToWin();
  else
    rzWinToImg();
  m_toolbarorientationchanged = false;
}

void KImageViewer::slot_toolBarOrientationChanged()
{
  m_toolbarorientationchanged = true;
}

void KImageViewer::rzWinToImg()
{
  // if the preferences say that the window shouldn't resize return
  if( ! m_resizeMode & ResizeWindow )
    return;

  // don't resize in FullScreen mode or when there's no image loaded
  if( m_bFullScreen || _canvas->isEmpty() )
    return;

  QSize desktop = kapp->desktop()->size();
  QRect workarea = m_kwinmodule->workArea();
  QRect frame = frameGeometry();
  QRect geom = geometry();

  int leftpanel = workarea.x();
  int rightpanel = desktop.width() - workarea.width() - leftpanel;
  int toppanel = workarea.y();
  int bottompanel = desktop.height() - workarea.height() - toppanel;

  int x = geom.x();
  int y = geom.y();

  int frameright = frame.width() + frame.x() - geom.width() - x;
  int framebottom = frame.height() + frame.y() - geom.height() - y;

  int width  = _canvas->contentsWidth();
  int height = _canvas->contentsHeight();
  sizeCorrection( width, height );

  int widthmax  = workarea.width()  - x - frameright;
  int heightmax = workarea.height() - y - framebottom;
  
  if( width > widthmax )
  {
    x = QMAX( x - frame.x(), x - ( width - widthmax ) ) + leftpanel;
    widthmax  = desktop.width()  - x - frameright - rightpanel;
  }
  if( height > heightmax )
  {
    y = QMAX( y - frame.y(), y - ( height - heightmax ) ) + toppanel;
    heightmax = desktop.height() - y - framebottom - bottompanel;
  }
  setGeometry( x, y, QMIN( width, widthmax ), QMIN( height, heightmax ) );
}

void KImageViewer::setupActions( KFilterList *filters )
{
  //setup the file menu
  KStdAction::open( this, SLOT( slot_load() ), actionCollection() );
  m_recent = KStdAction::openRecent(this, SLOT( slot_openRecent( const KURL& ) ), actionCollection());
  m_fileSave = KStdAction::save( this, SLOT( slot_save() ), actionCollection() );
  m_fileSaveAs = KStdAction::saveAs( this, SLOT( slot_saveAs() ), actionCollection() );
  m_fileClose = KStdAction::close( this, SLOT( slot_fileClose() ), actionCollection() );
  m_filePrint = KStdAction::print( this, SLOT( slot_printImage() ), actionCollection() );
  (void)new KAction( i18n( "&New Window" ), CTRL+Key_N, this, SLOT( slot_newViewer() ), actionCollection(), "new_window" );
  (void)new KAction( i18n( "Close &Window" ), CTRL+Key_X, this, SLOT( slot_closeViewer() ), actionCollection(), "close_window" );
  KStdAction::quit( this, SLOT( slot_quitApp() ), actionCollection() );

  //setup the edit menu
  m_editCrop = new KAction( i18n( "&Crop" ), Key_C, this, SLOT( slot_cropImage() ), actionCollection(), "crop" );
  m_editCrop->setEnabled( false );
  m_editReset = KStdAction::undo( this, SLOT( slot_reset() ), actionCollection() );
  m_editReset->setText( i18n( "&Reset" ) );
  m_editReset->setAccel( CTRL+Key_R );

  // zoom menu
  m_zoomMenu = new KActionMenu( i18n( "Z&oom" ), actionCollection(), "zoommenu" );
  m_zoom50 = new KAction( i18n( "&Half Size" ), ALT+Key_0, this, SLOT( slot_zoom50() ), actionCollection(), "zoom50" );
  m_zoomMenu->insert( m_zoom50 );
  m_zoom100 = new KAction( i18n( "&Normal Size" ), ALT+Key_1, this, SLOT( slot_zoom100() ), actionCollection(), "zoom100" );
  m_zoomMenu->insert( m_zoom100 );
  m_zoom200 = new KAction( i18n( "&Double Size" ), ALT+Key_2, this, SLOT( slot_zoom200() ), actionCollection(), "zoom200" );
  m_zoomMenu->insert( m_zoom200 );
  m_zoomMaxpect = new KAction( i18n( "&Fill Screen" ), ALT+Key_3, this, SLOT( slot_zoomMaxpect() ), actionCollection(), "zoomMaxpect" );
  m_zoomMenu->insert( m_zoomMaxpect );
  m_zoomMax = new KAction( i18n( "Fill &whole Screen" ), ALT+Key_4, this, SLOT( slot_zoomMax() ), actionCollection(), "zoomMax" );
  m_zoomMenu->insert( m_zoomMax );

  //setup the view menu
  m_viewZoom = KStdAction::zoom( this, SLOT( slot_zoomCustom() ), actionCollection() );
  m_viewZoomIn = KStdAction::zoomIn( this, SLOT( slot_zoomIn10() ), actionCollection() );
  m_viewZoomOut = KStdAction::zoomOut( this, SLOT( slot_zoomOut10() ), actionCollection() );
  m_viewDouble = new KAction( i18n( "&Double size" ), Key_BracketRight, this, SLOT( slot_zoomIn200() ), actionCollection(), "double_size" );
  m_viewHalf = new KAction( i18n( "&Half size" ), Key_BracketLeft, this, SLOT( slot_zoomOut50() ), actionCollection(), "half_size" );
  m_viewFullScreen = new KAction( i18n( "&Fullscreen Mode" ), "window_fullscreen", CTRL+SHIFT+Key_F, this, SLOT( slot_fullScreen() ), actionCollection(), "fullscreen" );
  m_imageSlide = new KAction( i18n( "&Slideshow On/Off" ), Key_S, _imageList, SLOT( slot_toggleSlideshow() ), actionCollection(), "slideshow" );

  // Go menu
  m_imagePrev = KStdAction::prior( _imageList, SLOT( slot_previous() ), actionCollection() );
  m_imageNext = KStdAction::next( _imageList, SLOT( slot_next() ), actionCollection() );
  m_imageFirst = KStdAction::firstPage( _imageList, SLOT( slot_first() ), actionCollection() );
  m_imageLast = KStdAction::lastPage( _imageList, SLOT( slot_last() ), actionCollection() );
  m_imagePrev->setText( i18n( "Previous Image" ) );
  m_imagePrev->setIcon( "back" );
  m_imageNext->setText( i18n( "Next Image" ) );
  m_imageNext->setIcon( "forward" );
  m_imageFirst->setText( i18n( "First Image" ) );
  m_imageLast->setText( i18n( "Last Image" ) );
  m_imageList = new KAction( i18n( "&Image List..." ), 0, this, SLOT( slot_toggleImageList() ), actionCollection(), "list" );

  // filter menu
  m_filterMenu = new KFilterAction( filters, i18n( "&Filter" ), actionCollection(), "filter" );

  // rotate menu
  m_rotateMenu = new KActionMenu( i18n( "&Rotate" ), actionCollection(), "rotate" );
  m_rotate90 = new KAction( i18n( "&90 Degrees" ), Key_Semicolon, this, SLOT( slot_rotate90() ), actionCollection(), "rotate90" );
  m_rotateMenu->insert( m_rotate90 );
  m_rotate180 = new KAction( i18n( "1&80 Degrees" ), 0, this, SLOT( slot_rotate180() ), actionCollection(), "rotate180" );
  m_rotateMenu->insert( m_rotate180 );
  m_rotate270 = new KAction( i18n( "2&70 Degrees" ), Key_Comma, this, SLOT( slot_rotate270() ), actionCollection(), "rotate270" );
  m_rotateMenu->insert( m_rotate270 );

  // flip menu
  m_flipMenu = new KActionMenu( i18n( "&Flip" ), actionCollection(), "flip" );
  m_flipV = new KAction( i18n( "&vertical" ), Key_V, this, SLOT( slot_flipVertical() ), actionCollection(), "flip_vertical" );
  m_flipMenu->insert( m_flipV );
  m_flipH = new KAction( i18n( "&horizontal" ), Key_H, this, SLOT( slot_flipHorizontal() ), actionCollection(), "flip_horizontal" );
  m_flipMenu->insert( m_flipH );

  //setup the desktop menu
  m_desktopMenu = new KActionMenu( i18n( "To &Desktop" ), actionCollection(), "desktop" );
  m_desktopTile = new KAction( i18n( "Desktop &Tile" ), 0, _canvas, SLOT( slot_tileToDesktop() ), actionCollection(), "desktop_tile" );
  m_desktopMenu->insert( m_desktopTile );
  m_desktopMax = new KAction( i18n( "Desktop &Max" ), 0, _canvas, SLOT( slot_maxToDesktop() ), actionCollection(), "desktop_max" );
  m_desktopMenu->insert( m_desktopMax );
  m_desktopMaxpect = new KAction( i18n( "Desktop Max&pect" ), 0, _canvas, SLOT( slot_maxpectToDesktop() ), actionCollection(), "desktop_maxpect" );
  m_desktopMenu->insert( m_desktopMaxpect );

  // image menu
  m_imageInfo = new KAction( i18n( "&Info" ), Key_I, this, SLOT( slot_showImageInfo() ), actionCollection(), "info" );

  // setup Settings menu
  m_settingsShowMenuBar = KStdAction::showMenubar( this, SLOT( slot_toggleMenuBar() ), actionCollection() );
  m_settingsShowToolBar = KStdAction::showToolbar( this, SLOT( slot_toggleToolBar() ), actionCollection() );
  m_settingsShowStatusBar = KStdAction::showStatusbar( this, SLOT( slot_toggleStatusBar() ), actionCollection() );
  KStdAction::saveOptions( this, SLOT( slot_saveOptions() ), actionCollection() );
  KStdAction::preferences( this, SLOT( slot_prefs() ), actionCollection() );
  KStdAction::keyBindings( this, SLOT( slot_confAccels() ), actionCollection() );
  KStdAction::configureToolbars( this, SLOT( slot_editToolbars() ), actionCollection() );

  m_editCrop->setEnabled( false );
  connect( _canvas, SIGNAL( selected( bool ) ), m_editCrop, SLOT( setEnabled( bool ) ) );

  connectFilters( filters );
  menuEntriesEnabled( false );

  QValueList<KAction*> actions = actionCollection()->actions();
  for (QValueList<KAction*>::ConstIterator it = actions.begin(); it != actions.end(); it++)
    (*it)->plugAccel(m_accel);

  createGUI();
}

void KImageViewer::slot_toggleMenuBar()
{
  if( menuBar()->isVisible() )
    menuBar()->hide();
  else
    menuBar()->show();

  if( m_resizeMode == ResizeImage )
    _canvas->slot_maxToWin();
  else
    rzWinToImg();
}

void KImageViewer::slot_editToolbars()
{
  saveMainWindowSettings( KGlobal::config(), "MainWindow" );
  KEditToolbar dlg( actionCollection() );
  connect( &dlg, SIGNAL( newToolbarConfig() ), this, SLOT( slot_newToolbarConfig() ) );

  if ( dlg.exec() )
    createGUI();
}

void KImageViewer::slot_newToolbarConfig() //This is called when OK or Apply is clicked
{
  applyMainWindowSettings( KGlobal::config(), "MainWindow" );
  createGUI();
}

void KImageViewer::slot_toggleToolBar()
{
  if( toolBar()->isVisible() )
    toolBar()->hide();
  else
    toolBar()->show();
  if( m_resizeMode == ResizeImage )
    _canvas->slot_maxToWin();
  else
    rzWinToImg();
}

void KImageViewer::slot_toggleStatusBar()
{
  if( statusBar()->isVisible() )
    statusBar()->hide();
  else
    statusBar()->show();
  if( m_resizeMode == ResizeImage )
    _canvas->slot_maxToWin();
  else
    rzWinToImg();
}

void KImageViewer::slot_showImageInfo()
{
  assert( m_imageinfo );
  m_imageinfo->show();
}

void KImageViewer::slot_cropImage()
{
  _canvas->slot_cropImage();
  rzWinToImg();
}

void KImageViewer::slot_zoom50()
{
  _mat.reset();
  if( m_resizeMode & FastScale )
  {
    _mat.scale( 0.5, 0.5 );
    _canvas->transformImage( _mat );
  }
  else
  {
    int width  = _canvas->originalSize().width()  / 2;
    int height = _canvas->originalSize().height() / 2;
    sizeCorrection( width, height );
    resize( width, height );
    _canvas->slot_maxToWin();
  }
  rzWinToImg();
}

void KImageViewer::slot_zoom100()
{
  _mat.reset();
  _mat.scale( 1, 1 );
  _canvas->transformImage( _mat );
  rzWinToImg();
}

void KImageViewer::slot_zoom200()
{
  _mat.reset();
  if( m_resizeMode & FastScale )
  {
    _mat.scale( 2, 2 );
    _canvas->transformImage( _mat );
  }
  else
  {
    int width  = _canvas->originalSize().width()  * 2;
    int height = _canvas->originalSize().height() * 2;
    sizeCorrection( width, height );
    resize( width, height );
    _canvas->slot_maxToWin();
  }
  rzWinToImg();
}

void KImageViewer::slot_zoomMaxpect()
{
  QRect workarea = m_kwinmodule->workArea();
  QRect frame = frameGeometry();
  QRect geom = geometry();
  QSize original = _canvas->originalSize();
  int width  = workarea.width() - frame.width() + geom.width();
  int height = workarea.height() - frame.height() + geom.height();
  sizeCorrection( width, height, false );
  double dh = (double)height / (double)original.height();
  double dw = (double)width  / (double)original.width();
  double d = QMIN( dh, dw );
  if( m_resizeMode & FastScale )
  {
    _mat.reset();
    _mat.scale( d, d );
    _canvas->transformImage( _mat );
    rzWinToImg();
  }
  else
  {
    width  = (int)( d * original.width()  );
    height = (int)( d * original.height() );
    sizeCorrection( width, height );
    setGeometry( workarea.x() + geom.x() - frame.x(), workarea.y() + geom.y() - frame.y(), width, height );
    _canvas->slot_maxToWin();
  }
}

void KImageViewer::slot_zoomMax()
{
  QRect workarea = m_kwinmodule->workArea();
  QRect frame = frameGeometry();
  QRect geom = geometry();
  int width  = workarea.width() - frame.width() + geom.width();
  int height = workarea.height() - frame.height() + geom.height();
  setGeometry( workarea.x() + geom.x() - frame.x(), workarea.y() + geom.y() - frame.y(), width, height );
  if( m_resizeMode & FastScale )
  {
    sizeCorrection( width, height, false );
    QSize original = _canvas->originalSize();
    double dh = (double)height / (double)original.height();
    double dw = (double)width  / (double)original.width();
    _mat.reset();
    _mat.scale( dw, dh );
    _canvas->transformImage( _mat );
  }
  else
    _canvas->slot_maxToWin();
}

void KImageViewer::sizeCorrection( int & width, int & height, bool add )
{
  if( toolBar()->isVisibleTo( this ) )
  {
    switch( toolBar()->barPos() )
    {
      case KToolBar::Top:
	height += ( kapp->style().guiStyle() == WindowsStyle ? 1 : 0 ) * ( add ? 1 : -1 );
      case KToolBar::Bottom:
        height += ( m_toolbarorientationchanged ? toolBar()->width() - 1 : toolBar()->height() ) * ( add ? 1 : -1 );
        break;
      case KToolBar::Left:
      case KToolBar::Right:
        width += ( m_toolbarorientationchanged ? toolBar()->height() + 1 : toolBar()->width() ) * ( add ? 1 : -1 );
        break;
      case KToolBar::Flat:
        height += kapp->style().toolBarHandleExtent() + ( kapp->style().guiStyle() == WindowsStyle ? 2 : 0 ) * ( add ? 1 : -1 );
        break;
    }
  }
  if( menuBar()->isVisibleTo( this ) && ( ! menuBar()->isTopLevelMenu() ) )
    height += menuBar()->heightForWidth( width ) * ( add ? 1 : -1 );
  if( statusBar()->isVisibleTo( this ) )
    height += statusBar()->height() * ( add ? 1 : -1 );
}

#include "viewer.moc"

