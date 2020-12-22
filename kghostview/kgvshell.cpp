#include <kiconloader.h>
#include <kstddirs.h>
#include <kapp.h>
#include <kaction.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <klibloader.h>
#include <kstdaction.h>

#include "kgv_miniwidget.h"
#include "kgv_view.h"
#include "kgvpageview.h"

#include "kgvshell.moc"

//TODO -- disable GUI when no file
//TODO -- don't stay open when no file, go directly to KFileDialog

KGVShell::KGVShell()
{
    m_gvpart = new KGVPart( false, this, "kgvpart", this, "kgvpart" );

    statedep = new KActionCollection( this, "statedep" );

    /*---- File -----------------------------------------------------------*/
    openact =
	    KStdAction::open( this, SLOT( slotFileOpen() ),
			      actionCollection() );
    recent =
	    KStdAction::openRecent( this, SLOT( openURL( const KURL& ) ),
				    actionCollection() );
    statedep->insert(
	    KStdAction::print( m_gvpart->miniWidget(), SLOT( print() ),
			       actionCollection() ) );
    (void)
	    KStdAction::quit( this, SLOT( slotQuit() ), actionCollection() );

    /*---- View -----------------------------------------------------------*/
    statedep->insert(
	    KStdAction::redisplay( m_gvpart->miniWidget(), SLOT( redisplay() ),
				   actionCollection()) );
    statedep->insert(
	    new KAction( i18n( "&Fit To Page Width" ), 0, this,
			 SLOT( slotFitToPage() ), actionCollection(),
			 "fit_to_page") );

    /*---- Settings -------------------------------------------------------*/
    showtoolbar =
	    KStdAction::showToolbar( this, SLOT(slotShowToolBar() ),
				     actionCollection());
    showstatusbar =
	    KStdAction::showStatusbar( this, SLOT(slotShowStatusBar() ),
				       actionCollection());

    // Just save them automatically is destructor. (TODO: of kgv_view!)
    //KStdAction::saveOptions ( this, SLOT (slotWriteSettings()),			    actionCollection());


    setXMLFile( "kghostviewui.rc" );

    // We could, at the user's option, make this connection and kghostview
    // will always resize to fit the width of the page.  But, for now,
    // let's not.
    // connect ( m_gvpart->widget(), SIGNAL (sizeHintChanged()),	    this, SLOT (slotResize ()) );

    setCentralWidget( m_gvpart->widget() );
    createGUI( m_gvpart );

    readSettings();
    enableStateDepActions( false );

    //TODO: use last size
    resize( width0, height0 );

    // Make sure the view has the keyboard focus.
    m_gvpart->widget()->setFocus();
}

KGVShell::~KGVShell()
{
    writeSettings();
    delete m_gvpart;
}

void
KGVShell::slotQuit()
{
    kapp->closeAllWindows();
}

void 
KGVShell::readProperties( KConfig *config )
{
    KURL url = config->readEntry( "URL" );
    if ( url.isValid() ) {
        openURL( url );
        m_gvpart->miniWidget()->goToPage( config->readNumEntry( "Current Page", 0 ));
        m_gvpart->miniWidget()->setMagstep( config->readNumEntry( "Magnification", 1 ));
    }
}

void 
KGVShell::saveProperties( KConfig *config )
{
    config->writeEntry( "URL", m_gvpart->url().prettyURL() );
    config->writeEntry( "Current Page", m_gvpart->miniWidget()->currentPage() );
    config->writeEntry( "Magnification", m_gvpart->miniWidget()->magnification() );
}

void
KGVShell::readSettings()
{
    KConfig* config = KGlobal::config();
    QString oldGroup = config->group();
    config->setGroup( "General" );
    showtoolbar->setChecked( config->readBoolEntry( "ShowToolBar", true ) );
    showstatusbar->setChecked( config->readBoolEntry( "ShowStatusBar", true ));
    width0 = config->readUnsignedNumEntry( "WindowWidth", 600 );
    height0 = config->readUnsignedNumEntry( "WindowHeight", 300 );
    config->setGroup( oldGroup );

    slotShowToolBar();
    slotShowStatusBar();

    recent->loadEntries( config );
}

void
KGVShell::writeSettings()
{
    KConfig *config = KGlobal::config();
    QString oldGroup = config->group();
    config->setGroup( "General" );
    config->writeEntry( "ShowToolBar", showtoolbar->isChecked() );
    config->writeEntry( "ShowStatusBar", showstatusbar->isChecked() );
    config->writeEntry( "WindowWidth", width() );
    config->writeEntry( "WindowHeight", height() );
    config->setGroup( oldGroup );

    recent->saveEntries( config );

    config->sync();
}

void
KGVShell::openURL( const KURL & url )
{
    if( m_gvpart->openURL( url ) ) {
	recent->addURL (url);
	enableStateDepActions( true );
    }
}

void
KGVShell::openStdin()
{
    if( m_gvpart->openStdin() )
	enableStateDepActions( true );
}

void KGVShell::slotFileOpen()
{
    KURL url = KFileDialog::getOpenURL( cwd, i18n(
		    "*.ps *.ps.gz *.eps *.eps.gz *.pdf|All document files\n"
		    "*.ps *.ps.gz|Postscript files\n"
		    "*.eps *.eps.gz|Encapsulated Postscript files\n"
		    "*.pdf|Portable Document Format files" ) );

    if( !url.isEmpty() )
	openURL( url );
}

void KGVShell::slotFitToPage()
{
    if( m_gvpart->pageView()->page() )
	resize( m_gvpart->pageView()->page()->width() +
		( width() - centralWidget()->width() ) +
		( m_gvpart->widget()->width() -
		  m_gvpart->pageView()->viewport()->width() )
		, height() );
}

void KGVShell::slotResize()
{
    resize( m_gvpart->pageView()->sizeHint().width(), height() );
}

void KGVShell::slotShowStatusBar()
{
    if( showstatusbar->isChecked() )
	statusBar()->show();
    else
	statusBar()->hide();
}

void KGVShell::slotShowToolBar()
{
    KToolBar* bar = toolBar( "mainToolBar" );

    if( bar ) {
	if( showtoolbar->isChecked() )
	    bar->show();
	else
	    bar->hide();
    }
}

void KGVShell::enableStateDepActions( bool enable )
{
    for( uint i = 0; i < statedep->count(); i++ )
	statedep->action( i )->setEnabled( enable );
}
