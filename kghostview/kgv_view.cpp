#include <qfileinfo.h>
#include <qframe.h>
#include <qlayout.h>

#include <kaction.h>
#include <kapp.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdirwatch.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <klocale.h>
#include <kstdaction.h>

#include "kgv_view.h"
#include "kgv_miniwidget.h"
#include "kgvpagedecorator.h"
#include "kgvpageview.h"
#include "kgvmainwidget.h"
#include "kpswidget.h"
#include "marklist.h"
#include "scrollbox.h"

#define PAGELIST_WIDTH 75

extern "C"{
  void *init_libkghostview()
  {
    return new KGVFactory;
  }
};

KInstance *KGVFactory::s_instance = 0L;

KGVFactory::KGVFactory()
{
}

KGVFactory::~KGVFactory()
{
    if ( s_instance )
        delete s_instance;

    s_instance = 0;
}

KParts::Part *KGVFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *className, const QStringList & )
{
    bool bBrowserView = ( strcmp( className, "Browser/View" ) == 0 );
    KParts::Part *obj = new KGVPart( bBrowserView, parentWidget, widgetName, parent, name );
    emit objectCreated( obj );
    return obj;
}

KInstance *KGVFactory::instance()
{
    if ( !s_instance )
        s_instance = new KInstance( "kghostview" );
    return s_instance;
}

KGVPart::KGVPart( bool bBrowserView, QWidget *parentWidget, 
		  const char *, QObject *parent, const char *name )
 :  KParts::ReadOnlyPart( parent, name )
{
    setInstance( KGVFactory::instance() );

    // Don't show the progress info dialog if we're embedded in Konqueror.
    showProgressInfo( !bBrowserView );

    // setenv( "GS_LIB", "/usr/local/kde/share/apps/kghostview", true );

    _watch = new KDirWatch;
    connect( _watch, SIGNAL( fileDirty( const QString& ) ),
	     this, SLOT( slotFileDirty( const QString& ) ) );
    
    // Setup main widget
    _mainWidget = new KGVMainWidget( parentWidget );
    _mainWidget->setFocusPolicy( QWidget::StrongFocus );
    connect( _mainWidget, SIGNAL( spacePressed() ),
             this, SLOT( slotReadDown() ) );

    QHBoxLayout* hlay = new QHBoxLayout( _mainWidget, 0, 0 );
    QVBoxLayout* vlay = new QVBoxLayout( hlay );
 
    _scrollBox = new ScrollBox( _mainWidget , "scrollbox" );
    _scrollBox->setMinimumWidth( PAGELIST_WIDTH );
    _scrollBox->setMinimumHeight( PAGELIST_WIDTH );
    vlay->addWidget( _scrollBox );

    _markList = new MarkList( _mainWidget, "marklist" );
    _markList->setMinimumWidth( PAGELIST_WIDTH );
    vlay->addWidget( _markList, 1 );

    _divider = new QFrame( _mainWidget, "divider" );
    _divider->setFrameStyle( QFrame::Panel | QFrame::Raised );
    _divider->setLineWidth( 1 );
    _divider->setMinimumWidth( 3 );
    hlay->addWidget( _divider );
 
    _pageView = new KGVPageView( _mainWidget, "pageview" );
    _pageView->viewport()->setBackgroundMode( QWidget::PaletteMid );
    hlay->addWidget( _pageView, 1 );
    _mainWidget->setFocusProxy( _pageView );

    setWidget( _mainWidget );

    KGVPageDecorator* pageDeco = new KGVPageDecorator( _pageView->viewport() );
    pageDeco->hide();
    KPSWidget* psWidget = new KPSWidget( pageDeco );
    _pageView->setPage( pageDeco );
    _docManager = new KGVMiniWidget( this );
    _docManager->setPSWidget( psWidget );
    
    // Connect up the necessary signals and slots.
    connect( _markList, SIGNAL( selected( int ) ), 
	     _docManager, SLOT( goToPage( int ) ) );
    connect( _docManager, SIGNAL( newPageShown( int ) ), 
	     _markList, SLOT( select( int ) ) );

    connect( _scrollBox, SIGNAL( valueChangedRelative( int, int ) ),
	     _pageView, SLOT( scrollBy( int, int ) ) );
    connect( _pageView, SIGNAL( pageSizeChanged( QSize ) ),
	     _scrollBox, SLOT( setPageSize( QSize ) ) );
    connect( _pageView, SIGNAL( viewSizeChanged( QSize ) ),
	     _scrollBox, SLOT( setViewSize( QSize ) ) );
    connect( _pageView, SIGNAL( contentsMoving( int, int ) ),
	     _scrollBox, SLOT( setViewPos( int, int ) ) );

    connect( _docManager, SIGNAL( fileChangeFailed() ),
	     this, SLOT( slotCancelWatch() ) );
    connect( _docManager, SIGNAL( setStatusBarText( const QString& ) ),
	     this, SIGNAL( setStatusBarText( const QString& ) ) );

    // Put state-dependent actions in this collection.
    statedep = new KActionCollection( this, "statedep", KGVFactory::instance() );
  
  
    // File Menu
    statedep->insert( KStdAction::saveAs( miniWidget(), SLOT( saveAs() ), 
			    actionCollection() ) );
    statedep->insert( new KAction( i18n( "Document &Info..." ), 0, 
			    miniWidget(), SLOT( info() ),
			    actionCollection(), "info") );


    // View Menu  
    QStringList orientations;
    orientations.append( i18n( "Auto" ) );
    orientations.append( i18n( "Portrait" ) );
    orientations.append( i18n( "Landscape" ) );
    orientations.append( i18n( "Upside Down" ) );
    orientations.append( i18n( "Seascape" ) );
  
    orientation = new KSelectAction( i18n( "&Orientation" ), 0, 0, 0, 
			actionCollection(), "orientation_menu" );
    orientation->setItems( orientations );
    connect( orientation, SIGNAL( activated( int ) ),
	     this, SLOT( slotOrientation( int ) ) );
  
    //TODO -- zoom (maybe remove media from toolbar)
    media = new KSelectAction( i18n( "Paper &Size" ), 0, 0, 0,
		       actionCollection(), "media_menu" );
    connect( media, SIGNAL( activated( int ) ),
	     this, SLOT( slotMedia(int ) ) );
  
    zoomInAct	= KStdAction::zoomIn (_docManager,  SLOT(zoomIn() ),
			actionCollection(), "zoomIn" );
			
    zoomOutAct	= KStdAction::zoomOut( _docManager,  SLOT( zoomOut() ),
			actionCollection(), "zoomOut" );
			
    backAct	= KStdAction::prior( this, SLOT( slotPrevPage() ),
			actionCollection(), "prevPage" );
			
    forwardAct	= KStdAction::next( this, SLOT( slotNextPage() ),
			actionCollection(), "nextPage" );
  
    startAct	= new KAction( i18n( "Go to start" ), "start",
			CTRL+Key_Home, this, SLOT( slotGotoStart() ), 
			actionCollection(), "goToStart" );
		    
    endAct	= new KAction( i18n( "Go to end" ), "finish",
			CTRL+Key_End, this, SLOT( slotGotoEnd() ), 
			actionCollection(), "goToEnd" );
		    
    readUpAct	= new KAction( i18n( "Read up document" ), "previous",
			SHIFT+Key_Space, this, SLOT( slotReadUp() ),
			actionCollection(), "readUp" );

    // don't specify Key_Space as Accel, it breaks the lineedit in konq! (Simon)
    readDownAct	= new KAction( i18n( "Read down document" ), "next",
			/*Key_Space*/0, this, SLOT( slotReadDown() ),
			actionCollection(), "readDown" );

    gotoAct	= KStdAction::gotoPage( _docManager, SLOT( goToPage() ),
			actionCollection(), "goToPage" );
    
    statedep->insert( orientation );
    statedep->insert( media );
    statedep->insert( zoomInAct );
    statedep->insert( zoomOutAct );
    statedep->insert( backAct );
    statedep->insert( forwardAct );
    statedep->insert( startAct );
    statedep->insert( endAct );
    statedep->insert( readUpAct );
    statedep->insert( readDownAct );
    statedep->insert( gotoAct);


    // Page Marks Menu
    statedep->insert(
	    new KAction( i18n( "Mark Current Page" ), "flag", CTRL+Key_M, 
		    _markList, SLOT( markCurrent() ), 
		    actionCollection(), "mark_current" ) );
    statedep->insert(  
	    new KAction( i18n( "Mark &All Pages" ), 0, 
		    _markList, SLOT( markAll() ),
		    actionCollection(), "mark_all" ) );
    statedep->insert( 
	    new KAction( i18n( "Mark &Even Pages" ), 0, 
		    _markList, SLOT( markEven() ),
		    actionCollection(), "mark_even" ) );
    statedep->insert( 
	    new KAction( i18n( "Mark &Odd Pages" ), 0, 
		    _markList, SLOT( markOdd() ),
		    actionCollection(), "mark_odd" ) );
    statedep->insert( 
	    new KAction( i18n( "&Toggle Page Marks" ), 0,
		    _markList, SLOT( toggleMarks() ),
		    actionCollection(), "toggle" ) );
    statedep->insert( 
	    new KAction( i18n("&Remove Page Marks"), 0, 
		    _markList, SLOT( removeMarks() ),
		    actionCollection(), "remove" ) );
      
  
    // TODO -- disable entry if there aren't any page names
    
    
    // Settings menu
    showscrollbars = new KToggleAction( i18n( "Show &Scrollbars" ), 0,
			this, SLOT( slotShowScrollBars() ),
			actionCollection(), "show_scrollbars" );
    
    watchaction	= new KToggleAction( i18n( "&Watch File" ), 0, 
			this, SLOT( slotWatchFile() ),
			actionCollection(), "watch_file" );

    showmarklist = new KToggleAction( i18n( "Show &Page List" ), 0, 
			this, SLOT( slotShowMarkList() ),
			actionCollection(), "show_page_list" );
  
    fancyAct = new KToggleAction (i18n("Show Page Names"), 0, 
			this, SLOT( slotFancy() ), 
			actionCollection(), "fancy_page_labels" );
    fancyAct->setChecked( _docManager->areFancyPageLabelsEnabled() );
    
    new KAction( i18n( "&Configure KGhostView..." ), "configure", 0, 
		miniWidget(), SLOT( configureGhostscript() ),
		actionCollection(), "configure" );

    //TODO
    // progress bars for conversions (see loadingProgress())
  
    _extension = new KGVBrowserExtension( this );
  
    setXMLFile( "kgv_part.rc" );
  
    connect( miniWidget(), SIGNAL( newPageShown( int ) ),
	     this, SLOT( slotNewPage( int ) ) );
    connect( _pageView, SIGNAL( contentsMoving( int, int ) ),
	     this, SLOT( slotPageMoved( int, int ) ) );

    readSettings();

    enableStateDepActions( false ); // no document to apply the actions to, yet
}

KGVPart::~KGVPart()
{
    delete _watch;
    writeSettings();
}

bool KGVPart::closeURL()
{
    _watch->removeDir( QFileInfo( m_file ).dirPath() );
    return KParts::ReadOnlyPart::closeURL();
}

void
KGVPart::writeSettings()
{
  KConfig *config = KGVFactory::instance()->config();
  QString oldGroup = config->group();
  config->setGroup( "General" );
  config->writeEntry ("ShowScrollBars", showscrollbars->isChecked());
  config->writeEntry ("WatchFile", watchaction->isChecked());
  config->writeEntry ("ShowPageList", showmarklist->isChecked());
  config->writeEntry ("ShowPageNames", fancyAct->isChecked());
  config->setGroup( oldGroup );

  miniWidget()->writeSettings();

  config->sync();
}

void
KGVPart::readSettings()
{
    KConfig *config = KGVFactory::instance()->config();
    QString oldGroup = config->group();
    config->setGroup( "General" );

    showscrollbars->setChecked( 
			config->readBoolEntry( "ShowScrollBars", true ) );
    slotShowScrollBars();

    watchaction->setChecked( config->readBoolEntry( "WatchFile", false ) );
    slotWatchFile();
  
    showmarklist->setChecked( config->readBoolEntry( "ShowPageList", true ) );
    slotShowMarkList();
  
    fancyAct->setChecked( config->readBoolEntry( "ShowPageNames", false ) );
    slotFancy();

    config->setGroup( oldGroup );
}

void KGVPart::slotScrollLeft()
{
    _pageView->scrollLeft();
}

void KGVPart::slotScrollRight()
{
    _pageView->scrollRight();
}

void KGVPart::slotScrollUp()
{
    _pageView->scrollUp();
}

void KGVPart::slotScrollDown()
{
    _pageView->scrollDown();
}

void KGVPart::slotReadUp()
{
    if( !_pageView->readUp() ) {
	_docManager->prevPage();
	_pageView->scrollBottom();
    }	
}

void KGVPart::slotReadDown()
{
    if( !_pageView->readDown() )
	slotNextPage();
}

void KGVPart::slotPrevPage()
{
    _docManager->prevPage();
    _pageView->scrollTop();
}

void KGVPart::slotNextPage()
{
    if( _docManager->nextPage() ) {
	_pageView->scrollTop();
    }
    else
	_pageView->scrollBottom();
}

void KGVPart::slotGotoStart()
{
    _docManager->firstPage();
    _pageView->scrollTop();
}

void
KGVPart::slotGotoEnd()
{
    _docManager->lastPage();
    _pageView->scrollTop();
}

void KGVPart::slotWatchFile()
{
    if( watchaction->isChecked() )
	_watch->startScan();
    else
	_watch->stopScan();
}

void KGVPart::slotCancelWatch()
{
    _watch->stopScan();
    watchaction->setChecked( false );
}

void KGVPart::enableStateDepActions( bool enable )
{
    int count = statedep->count();
    for ( int i = 0; i < count; i++ )
	statedep->action( i )->setEnabled( enable );

    if( true ) {
	QStringList items = miniWidget()->sizeList();
	items.prepend( i18n( "Auto ") );
	media->setItems( items );
    }
}

void KGVPart::updatePageDepActions()
{    
    if( _docManager->atFirstPage() ) {
	backAct->setEnabled( false );
	startAct->setEnabled( false );
    }
    else {
	backAct->setEnabled( true );
	startAct->setEnabled( true );
    }

    if( _docManager->atLastPage() ) {
	forwardAct->setEnabled( false );
	endAct->setEnabled( false );
    }
    else {
	forwardAct->setEnabled( true );
	endAct->setEnabled( true );
    }

    updateReadUpDownActions();
}

void KGVPart::updateReadUpDownActions()
{
    if( _docManager->atFirstPage() && _pageView->atTop() ) 
	readUpAct->setEnabled( false );
    else
	readUpAct->setEnabled( true );
    
    if( _docManager->atLastPage() && _pageView->atBottom() ) 
	readDownAct->setEnabled( false );
    else
	readDownAct->setEnabled( true );
}

bool KGVPart::openStdin()
{
    bool success = _docManager->openFile( "-" );
    if( success ) {
	_docManager->setOriginalURL( url() );
	_docManager->firstPage();	
	enableStateDepActions( true );  //do we want eAA (b) ?
    }
    return success;
}

bool KGVPart::openFile()
{
    bool success = _docManager->openFile( m_file );
    if( success ) {
	_docManager->setOriginalURL( url() );
	_docManager->firstPage();	
	connect( _pageView, SIGNAL( nextPage() ), SLOT( slotNextPage() ));
	connect( _pageView, SIGNAL( prevPage() ), SLOT( slotPrevPage() ));
	enableStateDepActions( true );
	_watch->addDir( QFileInfo( m_file ).dirPath() );
	slotWatchFile();
    }
    return success;
}

void KGVPart::slotFancy()
{
    _docManager->enableFancyPageLabels( fancyAct->isChecked() );
}

void KGVPart::slotFileDirty( const QString& fileName )
{
    if( fileName == m_file )
	_docManager->fileChanged( fileName );
}

void KGVPart::slotNewPage( int )
{
    updatePageDepActions();
  ;//media->setCurrentItem (miniWidget()->getSize()-1);
  ;//orientation->setCurrentItem (miniWidget()->getOrientation()-1);
  //TODO -- zoom
}

void KGVPart::slotPageMoved( int, int )
{
    updateReadUpDownActions();
}

void KGVPart::slotOrientation( int id )
{
    switch( id ) {
    case 0: 
	miniWidget()->restoreOverrideOrientation();
	break;
    case 1: 
	miniWidget()->setOverrideOrientation( KGV::PORTRAIT );
	break;
    case 2: 
	miniWidget()->setOverrideOrientation( KGV::LANDSCAPE );
	break;
    case 3: 
	miniWidget()->setOverrideOrientation( KGV::UPSIDEDOWN );
	break;
    case 4: 
	miniWidget()->setOverrideOrientation( KGV::SEASCAPE );
	break;
    default: ;
    }
}

void
KGVPart::slotMedia( int id )
{
    if( id == 0 )
	miniWidget()->restoreOverridePageMedia();
    else
	miniWidget()->setOverridePageMedia( id - 1 );
}

void KGVPart::slotShowScrollBars()
{
    _pageView->enableScrollBars( showscrollbars->isChecked() );
}

void
KGVPart::slotShowMarkList()
{
    if( showmarklist->isChecked() ) {
	_markList->show();
	_scrollBox->show();
	_divider->show();
    }
    else {
	_markList->hide();
	_scrollBox->hide();
	_divider->hide();
    }
}


KGVBrowserExtension::KGVBrowserExtension( KGVPart *parent ) :
  KParts::BrowserExtension( parent, "KGVBrowserExtension" )
{
    emit enableAction( "print", true );
    setURLDropHandlingEnabled( true );
}

void
KGVBrowserExtension::print()
{
  ((KGVPart *)parent())->miniWidget()->print();
}



#include "kgv_view.moc"

