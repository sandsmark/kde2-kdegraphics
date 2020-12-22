/****************************************************************************
**
** Copyright (C) 2000 by Daniel Duley <mosfet@kde.org>, David Sweet <dsweet@kde.org>
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

/**
 * This is essentially the KGhostview widget stripped down and without all the
 * KTMainWindow dependencies (there were quite a bit with menus, toolbars,
 * etc.. accessed all over the place). It would probably make sense to make
 * that use this in the future (ported by mosfet@kde.org).
 */

#include <qdragobject.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qpaintdevicemetrics.h>
#include <qwidget.h>

#include <kapp.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kfilterdev.h>
#include <kinstance.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimemagic.h>
#include <knotifyclient.h>
#include <kprinter.h>
#include <ktempfile.h>
#include <kio/netaccess.h>

#include <config.h>

#include "gotodialog.h"
#include "infodialog.h"
#include "kgv_view.h"
#include "version.h"

extern "C" {
#include "ps.h"
}

#include "kgv_miniwidget.h"

#define BUFFER_SIZE (8192)

KGVMiniWidget::KGVMiniWidget( KGVPart* parent, const char* name )
 : QObject( parent, name )
{   
    // Initialise all the variables declared in this class.
    // I ususally forget to do a few resulting in nasty seg. faults !
  
    _widget = parent->widget();
    page = 0;
    _fileName = "";
    psfile=0;
    _doc = 0;
    doc=0;
    mGotoDialog=0;
    _useFancyPageLabels = false;

    _tmpStdin	    = 0;
    _tmpUnzipped    = 0;
    _tmpFromPDF	    = 0;
    _tmpDSC	    = 0;

    _printer = new KPrinter;

    _currentPage = -1;
    shrink_magsteps = 10;
    expand_magsteps = 10;
    _magstep = 10;
  
    _marklist = parent->markList();

    // Get the screen resolution.
    QPaintDeviceMetrics qpdm( _widget );
    _defaultXdpi = qpdm.logicalDpiX();
    _defaultYdpi = qpdm.logicalDpiY();

    _defaultPageMedia = "A4";
  
    readSettings();
}

KGVMiniWidget::~KGVMiniWidget()
{
    if( _printer != 0 )
	delete _printer;
    if( mGotoDialog != 0 )
	delete mGotoDialog;
    clearTemporaryFiles();
}

bool KGVMiniWidget::openFile( QString name )
{
    kapp->processEvents();
    
    if( psfile ) 
        fclose( psfile );
    
    clearTemporaryFiles();
    
    // page->enableInterpreter();
    
    // Write the input received on stdin to the temporary file _tmpStdin.
    if( name == QString::fromLatin1( "-" ) ) {
	_tmpStdin = new KTempFile;
	CHECK_PTR( _tmpStdin );

	if( _tmpStdin->status() != 0 ) {
	    KMessageBox::error( _widget, 
		    i18n( "Could not create temporary file: %1" ) 
		    .arg( strerror( _tmpStdin->status() ) ) );
	    return false;
	}

	QByteArray buf( BUFFER_SIZE );
	int read = 0, wrtn = 0;
	while( ( read = fread( buf.data(), sizeof(char), buf.size(), stdin ) )
		> 0 ) {
	    wrtn = _tmpStdin->file()->writeBlock( buf.data(), read );
	    if( read != wrtn )
		break;
	    kapp->processEvents();
	}
	
	if( read != 0 ) {
	    KMessageBox::error( _widget, 
		    i18n( "Could not open standard input stream: %1" ) 
		    .arg( strerror( errno ) ) );
	    return false;
	}
	
	_tmpStdin->close();
	name = _tmpStdin->name();
    }

    if( !QFile::exists( name ) ) {
	KMessageBox::sorry( _widget, 
		i18n( "File <nobr><strong>%1</strong></nobr> doesn't exist" )
		.arg( name ) );
	return false;
    }

    QString mimetype( KMimeMagic::self()->findFileType( name )->mimeType() );
   
    // If the file is gzipped, unzip it to the temporary file _tmpUnzipped.
    if( mimetype == "application/x-gzip" ) {
	_tmpUnzipped = new KTempFile;
	CHECK_PTR( _tmpUnzipped );
	
	if( _tmpUnzipped->status() != 0 ) {
	    KMessageBox::error( _widget, 
		    i18n( "Could not create temporary file: %2" )
		    .arg( strerror( _tmpUnzipped->status() ) ) );
	    return false;
	}

	QIODevice* filterDev = KFilterDev::deviceForFile( name );
	CHECK_PTR( filterDev );
	
	if( !filterDev->open( IO_ReadOnly ) ) {
	    KMessageBox::error( _widget, 
		    i18n( "Could not unzip <nobr><strong>%1</strong></nobr>" ) 
		    .arg( name ) );
	    delete filterDev;
	    return false;
	}
	
	QByteArray buf( BUFFER_SIZE );
	int read = 0, wrtn = 0;
	while( ( read = filterDev->readBlock( buf.data(), buf.size() ) ) 
		> 0 ) {
	    wrtn = _tmpUnzipped->file()->writeBlock( buf.data(), read );
	    if( read != wrtn )
		break;
	    kapp->processEvents();
	}
	
	filterDev->close();
	delete filterDev;
	
	if( read != 0 ) {
	    KMessageBox::error( _widget, 
		i18n( "Could not unzip <nobr><strong>%1</strong></nobr>" ) 
		.arg( name ) );
	    return false;
	}

	_tmpUnzipped->close();
	name = _tmpUnzipped->name();
	mimetype = KMimeMagic::self()->findFileType( name )->mimeType();
    }
    
    // If the file contains a PDF document, create a DSC description file
    // of the PDF document. This can be passed to Ghostscript just like
    // an ordinary PS file.
    if( mimetype == "application/pdf" ) {
	_tmpDSC = new KTempFile( QString::null, ".ps" );
	CHECK_PTR( _tmpDSC );
	
	if( _tmpDSC->status() != 0 ) {
	    KMessageBox::error( _widget, 
		    i18n( "Could not create temporary file: %1" ) 
		    .arg( strerror( _tmpDSC->status() ) ) );
	    return false;
	}
	
	if( !pdf2dsc( name, _tmpDSC->name() ) ) {
	    KMessageBox::error( _widget,
		    i18n( "Error opening file <nobr><strong>%1</strong></nobr>: %2" )
		    .arg( name )
		    .arg( strerror( errno ) ) );
	    return false;
	}
	
	_tmpDSC->close();
	_pdfFileName = name;
	name = _tmpDSC->name();
	format = PDF;
    }
    else if( mimetype == "application/postscript" )
	format = PS;
    else {
	KMessageBox::sorry( _widget,
		i18n( "Could not open file <nobr><strong>%1</strong></nobr> which has " 
		      "type <strong>%2</strong>. KGhostview can only load "
		      "Postscript (.ps, .eps) and Portable Document Format " 
		      "(.pdf) files." )
		      .arg( name )
		      .arg( mimetype ) );
	return false;
    }
   
    FILE* fp = fopen( QFile::encodeName( name ), "r");
    if( fp == 0 ) {
	KMessageBox::error( _widget, 
		i18n( "Error opening file <nobr><strong>%1</strong></nobr>: %2" )
		.arg( name )
		.arg( strerror( errno ) ) );
	return false;
    } 
    else {
	_fileName = name;
	psfile = fp;
	bool result = setup();
	return result;
    }
}

void KGVMiniWidget::setPSWidget( KPSWidget* psWidget )
{
    page = psWidget;
    setMagstep( _magstep );
}

void KGVMiniWidget::info()
{
    QString ftitle, fdate;
    if( _doc ) {
	ftitle = _doc->title;
	fdate = _doc->creationDate;
    } 
    else {
	ftitle = i18n("Not known");
	fdate = ftitle;
    }
  
    mInfoDialog = new InfoDialog( _widget, "info", true );
    mInfoDialog->setup( _fileName, ftitle, fdate );
    mInfoDialog->exec();
    delete mInfoDialog;
}

void KGVMiniWidget::configureGhostscript()
{
    if( page->configure() )
	redisplay();
}

void KGVMiniWidget::readSettings()
{
    // Don't set the group here.
    KConfig *config = KGVFactory::instance()->config();

    _interpreterPath = config->readEntry( "Interpreter", "gs" );
    _useFancyPageLabels = config->readBoolEntry( "FancyPageLabels", false );

    int i = config->readNumEntry( "Magstep", 10 );
    if( i < 20 && i > 0 )
	_magstep = i;
  
    setMagstep( i );
}

void KGVMiniWidget::goToPage()
{
    /*
  //printf ("goToPage()\n");
  if (mGotoDialog==0)
    {
      mGotoDialog = new GotoDialog( this, "goto", false );
      connect( mGotoDialog, SIGNAL(gotoPage(int)), this, SLOT(goToPage(int)) );
    }
  
  mGotoDialog->setup(GotoDialogData(_currentPage+1,
				    num_parts+1, pages_in_part));
  mGotoDialog->show();
  */

}


void KGVMiniWidget::goToPage( int page )
{
    if( _currentPage != page ) {
	_currentPage = page;
	showPage( page );
    }
}

void KGVMiniWidget::saveAs()
{
    KURL saveURL = 
	    KFileDialog::getSaveURL( origurl.isLocalFile() ? origurl.url() : origurl.fileName(), 
                                     QString::null, _widget, QString::null );
    if( !KIO::NetAccess::upload( format == PDF ? _pdfFileName : _fileName, 
				 saveURL ) )
	; // TODO: Proper error dialog
}

void KGVMiniWidget::zoomIn()
{
    unsigned int i;
  
    i = _magstep + 1;
    if( i <= shrink_magsteps + expand_magsteps )
	setMagstep(i);
}

void KGVMiniWidget::zoomOut()
{
    int i;
  
    i = _magstep - 1;
    if( i >= 1 )
	setMagstep( (unsigned int)i );
}

void KGVMiniWidget::firstPage()
{
    goToPage( 0 );
}

void KGVMiniWidget::lastPage()
{
    goToPage( _doc->numberOfPages - 1 );
}

bool KGVMiniWidget::prevPage()
{
    int new_page = 0;
  
    if( _doc->isStructured() ) {
	new_page = _currentPage - 1;
	if( new_page < 0 )
	    return false;
    }
    
    goToPage( new_page );
    return true;
}

bool KGVMiniWidget::nextPage()
{
    int new_page = 0;
  
    if( _doc->isStructured() ) {
	new_page = _currentPage + 1;
	if( (unsigned int)new_page >= _doc->numberOfPages )
	    return false;
    }
  
    goToPage( new_page );
    return true;
}

/**
 * Returns a QString which contains a range representation of @p pageList.
 * Examples: [1,3]       -> "1,3"
 *           [1,2,3]     -> "1-3"
 *           [1,3,4,5,8] -> "1,3-5,8"
 */
QString KGVMiniWidget::pageListToRange( const QValueList<int>& pageList ) const
{
    QString range;
    int	    begin;
    int	    end;

    QValueList<int>::ConstIterator it = pageList.begin();
    while( it != pageList.end() ) {
	begin = *it;
	do {
	    end = *it;
	    ++it;
	} while( it != pageList.end() && end == (*it) - 1 );
	
	if( !range.isEmpty() )
	    range += ",";
	    
	if( begin == end )
	    range += QString::number( end );
	else
	    range += QString( "%1-%2" ).arg( begin ).arg( end );
    }

    return range;
}

void KGVMiniWidget::print()
{
    if( !_doc ) return;

    if( _doc->isStructured() ) {
	_printer->setPageSelection( KPrinter::ApplicationSide );
	_printer->setCurrentPage( _currentPage + 1 );
	_printer->setMinMax( 1, _doc->numberOfPages );
	_printer->setOption( "kde-range", 
			     pageListToRange( _marklist->markList() ) );
	if( _printer->setup( _widget ) ) {
	    KTempFile tf( QString::null, ".ps" );
	    if( tf.status() == 0 ) {
	        savePages( tf.name(), _printer->pageList() );
	        _printer->printFiles( QStringList( tf.name() ), true );
	    }
	    else {
	        // TODO: Proper error handling
	        ;
	    }
	}
    }
    else {
	_printer->setPageSelection( KPrinter::SystemSide );
	if( _printer->setup( _widget ) ) 
	    _printer->printFiles( _fileName );
    }
}

bool KGVMiniWidget::savePages( const QString& saveFileName, 
                               const QValueList<int>& pageList )
{
    QString inputFile;

    if( format == PDF ) {
	if( convertFromPDF() )
	    inputFile = _tmpFromPDF->name();
	else
	    // return i18n( "Could not convert file from PDF to PS format." );
	    return false;
    }
    else
	inputFile = _fileName;

    psCopyDoc( inputFile, saveFileName, pageList );
    
    return true;
}

// length calculates string length at compile time
// can only be used with character constants
#define length( a ) ( sizeof( a ) - 1 )

// Copy the headers, marked pages, and trailer to fp

bool KGVMiniWidget::psCopyDoc( const QString& inputFile, 
	const QString& outputFile, const QValueList<int>& pageList )
{
    FILE* from;
    FILE* to;
    char text[ PSLINELENGTH ];
    char* comment;
    bool pages_written = false;
    bool pages_atend = false;
    unsigned int i = 0;
    unsigned int pages = 0;
    long here;
 
    kdDebug(4500) << "KGVMiniWidget: Copying pages from " << inputFile << " to "
    << outputFile << endl;
 
    from = fopen( QFile::encodeName( inputFile ), "r" );
    to = fopen( QFile::encodeName( outputFile ), "w" );
    
    pages = pageList.count();

    if( pages == 0 ) {
	KMessageBox::error( 0,
		i18n( "Printing failed because the list of\n"
				"pages to be printed was empty.\n" ),
			  i18n( "Error printing" ) );
	return false;
    }

    KDSC::Document* pdfdoc = 0; 
    struct document *ldoc;

    if( format == PS )
	ldoc = doc;
    else {
	pdfdoc = KDSC::scan( inputFile );
	if( !pdfdoc )
	    return false;

	ldoc = pdfdoc->_doc;
    }

    here = ldoc->beginheader;
    while( ( comment = pscopyuntil( from, to, here,
	     ldoc->endheader, "%%Pages:" ) ) ) {
	here = ftell( from );
	if( pages_written || pages_atend ) {
	    free( comment );
	    continue;
	}
	sscanf( comment + length("%%Pages:" ), "%256s", text );
        text[256] = 0;
	if( strcmp( text, "(atend)" ) == 0 ) {
	    fputs( comment, to );
	    pages_atend = true;
	} 
	else {
	    switch ( sscanf( comment + length( "%%Pages:" ), "%*d %d", &i ) )  {
	    case 1:
		fprintf( to, "%%%%Pages: %d %d\n", pages, i );
		break;
	    default:
		fprintf( to, "%%%%Pages: %d\n", pages );
		break;
	    }
	    pages_written = true;
	}
	free(comment);
    }
    pscopy( from, to, ldoc->beginpreview, ldoc->endpreview );
    pscopy( from, to, ldoc->begindefaults, ldoc->enddefaults );
    pscopy( from, to, ldoc->beginprolog, ldoc->endprolog );
    pscopy( from, to, ldoc->beginsetup, ldoc->endsetup );
  
    //TODO -- Check that a all ldoc attributes are copied

    unsigned int count = 1;
    QValueList<int>::ConstIterator it;
    for( it = pageList.begin(); it != pageList.end(); ++it ) {
	i = (*it) - 1;
	comment = pscopyuntil( from, to, ldoc->pages[i].begin,
			       ldoc->pages[i].end, "%%Page:" );
	free( comment );
	fprintf( to, "%%%%Page: %s %d\n", ldoc->pages[i].label, 
		 count++ );
	pscopy( from, to, -1, ldoc->pages[i].end );
    }

    here = ldoc->begintrailer;
    while( ( comment = pscopyuntil( from, to, here,
				  ldoc->endtrailer, "%%Pages:" ) ) ) {
	here = ftell( from );
	if ( pages_written ) {
	    free( comment );
	    continue;
	}
	switch ( sscanf( comment + length( "%%Pages:" ), "%*d %d", &i ) ) {
	case 1:
	    fprintf( to, "%%%%Pages: %d %d\n", pages, i );
	    break;
	default:
	    fprintf( to, "%%%%Pages: %d\n", pages );
	    break;
	}
	pages_written = true;
	free( comment );
    }
    
    fclose( from ); 
    fclose( to );

    if( format == PDF )
	delete pdfdoc;

    return true;
}

#undef length


bool KGVMiniWidget::pdf2dsc( const QString& pdfName, const QString& dscName )
{
    KProcess process;
    process << _interpreterPath
	    << "-dSAFER"
	    << "-dPARANOIDSAFER"
	    << "-dDELAYSAFER"
	    << "-dNODISPLAY" 
	    << "-dQUIET" 
	    << QString( "-sPDFname=%1" ).arg( pdfName )
	    << QString( "-sDSCname=%1" ).arg( dscName )
            << "-c"
            << "<< /PermitFileReading [ PDFname ] /PermitFileWriting [ DSCname ] /PermitFileControl [] >> setuserparams .locksafe"
            << "-f"
	    << "pdf2dsc.ps"
	    << "-c"
	    << "quit";

    connect( &process, SIGNAL( processExited( KProcess* ) ),
	     this, SLOT( pdf2dscExited() ) );

    kdDebug(4500) << "KGVMiniWidget: pdf2dsc started" << endl;
    process.start( KProcess::NotifyOnExit );
    kapp->enter_loop();

    if( !process.normalExit() || process.exitStatus() != 0 )
	return false;
    else
	return true;
}

void KGVMiniWidget::pdf2dscExited()
{
    kdDebug(4500) << "KGVMiniWidget: pdf2dsc exited" << endl;
    kapp->exit_loop();
}

void KGVMiniWidget::fileChanged( const QString& name )
{
    kdDebug(4500) << "KGVMiniWidget: fileChanged " << name << endl;
    
    if( !psfile )
	return;

    unsigned int savepage = _currentPage;
 
    if( openFile( name ) ) 
	goToPage( savepage );
    else 
	emit fileChangeFailed();
}

void KGVMiniWidget::redisplay ()
{
    if( !psfile )
	return;

    page->disableInterpreter();
    showPage( _currentPage );
}

void KGVMiniWidget::restoreOverrideOrientation()
{
    _overrideOrientation.setNull();
    showPage( _currentPage );
}

void KGVMiniWidget::setOverrideOrientation( KGV::Orientation orientation )
{
    _overrideOrientation = orientation;
    showPage( _currentPage );
}

KGV::Orientation KGVMiniWidget::orientation() const
{
    kdDebug(4500) << "KGVMiniWidget::orientation()" << endl;
    
    KGV::OrientationM o = _overrideOrientation;
    if( !o.isNull() ) 
	return o.data();
    o = _doc->orientation;
    if( !o.isNull() ) 
	return o.data();
	
    if( !_doc->boundingBox.isNull() 
          && _doc->boundingBox->width() > _doc->boundingBox->height() )
        return KGV::LANDSCAPE;
    else
        return KGV::PORTRAIT;
}

KGV::Orientation KGVMiniWidget::orientation( int pagenumber ) const
{
    kdDebug(4500) << "KGVMiniWidget::orientation( " << pagenumber << " )" << endl;
    
    KGV::OrientationM o = _overrideOrientation;
    if( !o.isNull() )
        return o.data();
    o = _doc->pageOrientation( pagenumber );
    if( !o.isNull() )
        return o.data();
    
    // No orientation specified in document
    if( !_doc->isEPSF )
        return KGV::PORTRAIT;
    else if( !_doc->boundingBox.isNull() 
          && _doc->boundingBox->width() > _doc->boundingBox->height() )
        return KGV::LANDSCAPE;
    else
        return KGV::PORTRAIT;
}

void KGVMiniWidget::restoreOverridePageMedia()
{
    _overridePageMedia = QString::null;
    showPage( _currentPage );
}

void KGVMiniWidget::setOverridePageMedia( const QString& pageMedia )
{
    _overridePageMedia = pageMedia;
    showPage( _currentPage );
}

void KGVMiniWidget::setOverridePageMedia( int pageMediaIndex )
{
    if( _doc )
	setOverridePageMedia( _doc->documentMedia[ pageMediaIndex ] );
}

QString KGVMiniWidget::pageMedia() const
{
    QString p = _overridePageMedia;
    if( !p.isNull() )
	return p;
    p = _doc->defaultPageMedia;
    if( !p.isNull() )
	return p;

    return _defaultPageMedia;
}

QString KGVMiniWidget::pageMedia( int pagenumber ) const
{  
    QString p;

    p = _overridePageMedia;
    if( !p.isNull() )
	return p;
    p = _doc->pageMedia( pagenumber );
    if( !p.isNull() )
	return p;

    // If no mediatype specified in document.
    return _defaultPageMedia;
}

bool KGVMiniWidget::atFirstPage() const
{
    return _currentPage == 0;
}

bool KGVMiniWidget::atLastPage() const
{
    return _currentPage == static_cast< int >( _doc->numberOfPages ) - 1;
}

KGV::BoundingBox KGVMiniWidget::computeBoundingBox( const QString& pageMedia )
{
    if( _doc->isEPSF 
       //Ignore malformed bounding boxes 
     && _doc->boundingBox->urx() > _doc->boundingBox->llx()
     && _doc->boundingBox->ury() > _doc->boundingBox->lly() ) {
	return _doc->boundingBox.data();
    } 
    else {
	int llx, lly, urx, ury;
	llx = lly = 0;
	urx = _doc->paperSizes[ pageMedia ].width();
	ury = _doc->paperSizes[ pageMedia ].height();
	return KGV::BoundingBox( llx, lly, urx, ury );
    }
}

QSize KGVMiniWidget::computePageSize( const QString& pageMedia )
{
    return _doc->paperSizes[ pageMedia.lower() ];
/*
    if( _doc->isEPSF 
       //Ignore malformed bounding boxes 
     && _doc->boundingBox->urx() > _doc->boundingBox->llx()
     && _doc->boundingBox->ury() > _doc->boundingBox->lly() ) {
	// return _doc->boundingBox.data();
	return QSize( _doc->boundingBox->width(),
		      _doc->boundingBox->height() );
    } 
    else 
	return _doc->paperSizes[ pageMedia ];
*/
}

void KGVMiniWidget::showPage( int pagenumber )
{
    if( !page || _fileName.isEmpty() ) {
	return;
    }

    kdDebug(4500) << "KGVMiniWidget::showPage( " << pagenumber << " )" << endl;

    static_cast< QWidget* >( page->parent() )->show();

    // Coerce page number to fall in range
    if( _doc->isStructured() ) {
	if( ( unsigned int)pagenumber >= _doc->numberOfPages ) {
	    pagenumber = _doc->numberOfPages - 1;
	}    
	if( pagenumber < 0 ) {
	    pagenumber = 0;
	}
    }
    
    if( _doc->isStructured() ) {
	page->setOrientation( orientation( pagenumber ) );
	QSize size = computePageSize( pageMedia( pagenumber ) );
	page->setPageSize( size );
    }
    else { 
	page->setOrientation( orientation() );
	QSize size = computePageSize( pageMedia() );
	page->setPageSize( size );
    }

    page->layout();
    
    // Send the page to the interpreter.
    if( _doc->isStructured() ) {
	_currentPage = pagenumber;
	if( page->isInterpreterReady() ) { 
	    // Interpreter ready - Fire off next page
	    page->nextPage();
	}
	else {
	    // Start interpreter and send preamble
	    page->enableInterpreter();
	    if( !_doc->prologOffset.isNull() )
		page->sendPS( psfile, _doc->prologOffset.data(), false );
	    if( !_doc->setupOffset.isNull() )
		page->sendPS( psfile, _doc->setupOffset.data(),  false );
	}
	// Send current page
	page->sendPS( psfile, _doc->pages[ _currentPage ]->offset, false );
    } 
    else {
	if( !page->isInterpreterRunning() ) {
	    // This is not a structured document -- start interpreter
	    page->enableInterpreter();
	    if( !doc )
		page->disableInterpreter();
	}
	else if( page->isInterpreterReady() ) {
	    page->nextPage();
	}
	else {
	    /*
	    KNotifyClient::userEvent 
	      (i18n("KGhostview cannot load the document, \"%1\".\n"
		    "It appears to be broken.").arg( _fileName ),
	       KNotifyClient::Messagebox);
	    page->disableInterpreter();
	    psfile=0;

	    //TODO: More to do to turn off display?
	    */
	    return;
	}
    }
    
    //
    // Well that takes care of sending the postscript.
    // Now update the page label on the status line
    //
    if( _doc->isStructured() ) {
	page_string = i18n( "Page" );
	if( pagenumber == -1 )
	    page_string += QString( " 1 " );
	else
	    if( _useFancyPageLabels ) {
		QString label( _doc->pages[ _currentPage ]->label );
		page_string += QString( " %1 \"%2\" " ).arg( pagenumber + 1 )
						       .arg( label );
	    }
	    else
		page_string += QString( " %1 " ).arg( pagenumber + 1 );
	
	page_label_text=page_string + page_total_label;
	
	emit setStatusBarText( page_label_text );
    }

    emit newPageShown( pagenumber );
}

bool KGVMiniWidget::setup()
{
    page->disableInterpreter();

    if( _fileName.isEmpty() ) 
	return false;
    
    // return to document defaults
    _overrideOrientation.setNull();
    _overridePageMedia = QString::null;
    emit setStatusBarText( "" );
 
    // Reset to a known state.
    delete _doc;
    _doc = 0;
    doc = 0;
    _currentPage = -1;

    // Scan document and start setting things up
    if( psfile ) {
	_doc = KDSC::scan( _fileName );
	if( !_doc )
	    return false;

	// Hmmm, weird :-)
	// Until the KDSC classes are complete we still need doc.
	doc = _doc->_doc;
    }
  
    buildTOC();
    
    return true;
}

void KGVMiniWidget::buildTOC()
{
    // Build table of contents
    // Well, that's what it used to be called !!
    
    int this_page, last_page=0;

    _marklist->setAutoUpdate( false );
    _marklist->clear();
    
    if( _doc->isStructured() ) {
	unsigned int maxlen = 0;
	unsigned int i, j;
     
	// Find the longest pagelabel
	if(  _useFancyPageLabels ) {
	    for( i = 0; i < _doc->numberOfPages; i++ )
		maxlen = QMAX( maxlen, _doc->pages[ i ]->label.length() );
	}
	else {
	    double x;
	    x = _doc->numberOfPages;
	    maxlen = (int)( log10(x) + 1 );
	}
	
	toc_entry_length = maxlen + 3;
	toc_length = _doc->numberOfPages * toc_entry_length - 1;
      
	if( _useFancyPageLabels )
	    for( i = 0; i < _doc->numberOfPages; i++ ) {
		if( doc->pageorder == DESCEND )
		    j = ( _doc->numberOfPages - 1 ) - i;
		else 
		    j = i;
		this_page = _doc->pages[ j ]->label.toInt();
		last_page = this_page;
	    } 
	
	// Set 0 filename for gs -- use pipe.
	page->setFileName( format == PDF ? _pdfFileName : _fileName, true );
      
	// finally set marked list
	QString s, tip;
	for( i = 1; i <= _doc->numberOfPages; i++ ) {
	    j = _doc->numberOfPages - i;
	    tip = _doc->pages[ j ]->label;

	    if( !_useFancyPageLabels )
		s.setNum (j+1);
	    else
		s=tip;

	    _marklist->insertItem( s, 0, tip );
	}
    } 
    else {
	toc_length = 0;
	toc_entry_length = 3;
	page->setFileName( _fileName, false );
	QString s("1");
	_marklist->insertItem( s, 0 );
    }
   
    _marklist->setAutoUpdate( true );
    _marklist->update();
   
    if( doc ) {
	if( _doc->numberOfPages == 0 )
	    page_total_label = i18n("of 1    ");
	else if( _doc->numberOfPages > 0 && _doc->numberOfPages < 10 ) 
	    page_total_label = i18n("of %1    ").arg( _doc->numberOfPages );
	else if( _doc->numberOfPages < 100 ) 
	    page_total_label = i18n("of %1  ").arg( _doc->numberOfPages );
	else if( doc ) 
	    page_total_label = i18n("of %1").arg( _doc->numberOfPages );
	else 
	    page_total_label = "	  ";
    }
}

void KGVMiniWidget::setMagstep( unsigned int magstep )
{
    if( !page )
	return;

    float xdpi, ydpi;
    xdpi = _defaultXdpi;
    ydpi = _defaultYdpi;
    _magstep = magstep;
    
    magnify( xdpi, _magstep );
    magnify( ydpi, _magstep );
    page->setResolution( xdpi, ydpi );
    showPage( _currentPage );
}

void
KGVMiniWidget::magnify( float& dpi, unsigned int magstep )
{
    if( magstep < shrink_magsteps )
	dpi = (int)( dpi * magstep / shrink_magsteps );
    else
	dpi = (int)( dpi + 2 * dpi * ( magstep - shrink_magsteps )/ expand_magsteps );
}

void
KGVMiniWidget::setOriginalURL (const KURL &url)
{
  origurl = url;
}

void
KGVMiniWidget::writeSettings()
{
    KConfig* config = KGVFactory::instance()->config();
    QString oldGroup = config->group();
    config->setGroup( "KGVMiniWidget" );   
    config->writeEntry( "FancyPageLabels", _useFancyPageLabels );
    config->setGroup( oldGroup );
    
    if( page )
	page->writeSettings(); //calls config->sync()
}

bool KGVMiniWidget::convertFromPDF()
{
    if( !_tmpFromPDF ) {
	_tmpFromPDF = new KTempFile( QString::null, ".ps" );
	
	if( _tmpFromPDF->status() != 0 ) {
	    KMessageBox::error( _widget,
		    i18n( "Could not create temporary file: " ) 
		    .arg( strerror( _tmpStdin->status() ) ) );
	    return false;
	}
	
	const char *cmd_convert_pdf = 
		"%s -q -dNOPAUSE -dBATCH -dSAFER -dPARANOIDSAFER -sDEVICE=pswrite -sOutputFile=%s -c save pop -f %s";
	QCString cmd;
	cmd.sprintf( cmd_convert_pdf, _interpreterPath.local8Bit().data(),
		QFile::encodeName( _tmpFromPDF->name() ).data(), 
		QFile::encodeName( KShellProcess::quote(_pdfFileName) ).data() );
  
	// TODO -- timeout/fail on this conversion (it can hang on a bad pdf)
	// TODO -- use output from gs (leave out -q) to drive a progress bar
  
	kdDebug(4500) << "Executing command: " << cmd << endl;
	int r = system( cmd );
	
	if( r ) {
	    // TODO -- error message (can't open, strerr())
	    return false;
	}
    }
    
    return true;
}

void KGVMiniWidget::enableFancyPageLabels( bool e )
{
    if( _useFancyPageLabels != e ) {
	_useFancyPageLabels = e;
	if( _doc ) {
	    buildTOC();
	    showPage( _currentPage );
	}
    }
}

void KGVMiniWidget::clearTemporaryFiles()
{
    if( _tmpStdin ) {
	_tmpStdin->setAutoDelete( true );
	delete _tmpStdin;
	_tmpStdin = 0;
    }	
    if( _tmpUnzipped ) {
	_tmpUnzipped->setAutoDelete( true );
	delete _tmpUnzipped;
	_tmpUnzipped = 0;
    }
    if( _tmpFromPDF ) {
	_tmpFromPDF->setAutoDelete( true );
	delete _tmpFromPDF;
	_tmpFromPDF = 0;
    }
    if( _tmpDSC ) {
	_tmpDSC->setAutoDelete( true );
	delete _tmpDSC;
	_tmpDSC = 0;
    }
}

#include "kgv_miniwidget.moc"

