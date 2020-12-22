/****************************************************************************
**
** A Qt PostScript widget.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <config.h>

#include <qdrawutil.h>
#include <qpaintdevicemetrics.h>
#include <qstringlist.h>

#include <kapp.h>
#include <kdebug.h>

#include "kgvconfigdialog.h"
#include "kpswidget.h"
#include "kpswidget.moc"

#define BUFFER_SIZE (8192)

int handler(Display *d, XErrorEvent *e)
{
   char msg[80], req[80], number[80];

   XGetErrorText(d, e->error_code, msg, sizeof(msg));
   sprintf(number, "%d", e->request_code);
   XGetErrorDatabaseText(d, "XRequest", number, "<unknown>", req, sizeof(req));

   //fprintf(stderr, "kghostview: %s(0x%lx): %s\n", req, e->resourceid, msg);

   return 0;
}

KPSWidget::KPSWidget( QWidget *parent, const char* name )
 : QWidget( parent, name )
{
    _mWin = None;
    _process = 0;

    XSetErrorHandler(handler);

    gs_window = winId();
    gs_display = x11Display();

    // Get the screen resolution.
    QPaintDeviceMetrics qpdm( this );
    _baseXdpi = qpdm.logicalDpiX();
    _baseYdpi = qpdm.logicalDpiY();
 
    messages = new MessagesDialog( 0, "messages" );
    intConfig = new KGVConfigDialog( topLevelWidget(), "intConfig" );

    // Initialise class variables
    // I usually forget a few important ones resulting in nasty seg. faults.

    _backgroundPixmap = None;

    left_margin = 0; right_margin = 0; bottom_margin = 0; top_margin = 0;
    _xdpi=75.0; _ydpi=75.0;

    show_messages=false;

    input_buffer = 0;
    bytes_left = 0;
    _inputQueue.setAutoDelete( true );
    _orientation = KGV::PORTRAIT;
    
    _interpreterBusy	 = false;
    _stdinReady		 = false;
    _interpreterReady	 = false;
    _interpreterDisabled = false;
    _layoutDirty	 = false;
   
#define GSATOM(name) (Atom) XInternAtom( gs_display, name, false );
    ghostview		= GSATOM( "GHOSTVIEW" );
    ghostview_colors	= GSATOM( "GHOSTVIEW_COLORS" );
    next		= GSATOM( "NEXT" );
    page		= GSATOM( "PAGE" );
    done		= GSATOM( "DONE" );
   
    setBackgroundMode( PaletteBase );
    layout();
}

KPSWidget::~KPSWidget()
{
    if( _process ) delete _process; 
    _process = 0;
    stopInterpreter();
    if( _backgroundPixmap != None ) 
	XFreePixmap( gs_display, _backgroundPixmap );
    delete intConfig;
    delete messages;
}


// ************************************************************************
//
//	PUBLIC METHODS
//
// ************************************************************************

/**
 * Allow the interpreter to start, and try to start it.
 */
void KPSWidget::enableInterpreter()
{
    _interpreterDisabled = false;
    startInterpreter();
}

/**
 * Stop any running interpreter and don't allow new ones to start.
 */
void KPSWidget::disableInterpreter()
{
    _interpreterDisabled = true;
    stopInterpreter();
}

/**
 * Returns true if the interpreter is ready for new input.
 */
bool KPSWidget::isInterpreterReady() const
{
    return isInterpreterRunning() && _interpreterReady;
}

/**
 * Returns true if the interpreter is running.
 */
bool KPSWidget::isInterpreterRunning() const
{
    return ( _process && _process->isRunning() );
}

/**
 * Tell ghostscript to start the next page.
 * Returns false if ghostscript is not running, or not ready to start
 * another page.
 * If another page is started, sets the _interpreterReady flag and cursor.
 */
bool KPSWidget::nextPage()
{
    XEvent ev;

    if( !isInterpreterRunning() )
	return false;
	
    if( _mWin == None ) {
	kdDebug(4500) << "kghostview: communication window unknown!" << endl;
	return false;
    }
	
    if( _interpreterReady ) {
	_interpreterReady = false;
	setCursor( waitCursor );

	ev.xclient.type = ClientMessage;
	ev.xclient.display = gs_display;
	ev.xclient.window = _mWin;
	ev.xclient.message_type = next;
	ev.xclient.format = 32;
		
	XSendEvent( gs_display, _mWin, false, 0, &ev );
	XFlush( gs_display );

	return true;
    }
    else
	return false;
}

/**
 * Queue a portion of a PostScript file for output to ghostscript and start
 * processing the queue.
 *
 * fp: FILE* of the file in question. NOTE: if you have several KPSWidget's
 * reading from the same file, you must open a unique FILE* for each widget.
 * begin: position in file to start.
 * len: number of bytes to write.
 *
 * If an interpreter is not running, nothing is queued and false is returned.
 */
bool KPSWidget::sendPS( FILE* fp, const KGV::FileOffset& offset, bool close )
{
    if( !isInterpreterRunning() )
	return false;

    // Create a new record to add to the queue.
    Record* ps_new = new Record;
    ps_new->fp = fp;
    ps_new->begin = offset.begin();
    ps_new->len = offset.length();
    ps_new->seek_needed = true;
    ps_new->close = close;

    // TODO: move this somewhere else.
    if( input_buffer == 0 )
	input_buffer = (char *) malloc(BUFFER_SIZE);

    if( _inputQueue.isEmpty() )
	bytes_left = offset.length();

    // Queue the record.
    _inputQueue.enqueue( ps_new );

    // Start processing the queue.
    if( _stdinReady )
	gs_input();

    return true;
}

/**
 * Sets the filename of the ghostscript input. Usually we use a pipe for
 * communication and no filename will be needed.
 */
void KPSWidget::setFileName( const QString& fileName, bool usePipe )
{
    _fileName = fileName;
    _usePipe = usePipe;
}

void KPSWidget::setOrientation( KGV::Orientation orientation )
{
    if( _orientation != orientation ) {
	_orientation = orientation;
	_layoutDirty = true;
    }
}

void KPSWidget::setBoundingBox( const KGV::BoundingBox& boundingBox )
{
    if( _boundingBox != boundingBox ) {
	_boundingBox = boundingBox;
	_layoutDirty = true;
    }
}

void KPSWidget::setPageSize( const QSize& pageSize )
{
    if( _pageSize != pageSize ) {
	_pageSize = pageSize;
	_layoutDirty = true;
    }
}

void KPSWidget::setResolution( int xdpi, int ydpi )
{
    if( _xdpi != xdpi || _ydpi != ydpi ) {
	_xdpi = xdpi; _ydpi = ydpi;
	_layoutDirty = true;
    }
}

void KPSWidget::layout()
{
    kdDebug(4500) << "KPSWidget::layout()" << endl;
    
    if( _layoutDirty ) {
	setup();
	repaint();
	_layoutDirty = false;
    }
}

/*****************************************************************************
 *
 *	SLOTS
 *
 ****************************************************************************/

bool KPSWidget::configure()
{
   intConfig->setup();
   if (intConfig->exec())
   {
      setup();
      return true;
   }

   return false;
}

void
KPSWidget::writeSettings()
{
    intConfig->writeSettings();
}

// ************************************************************************
//
//	PROTECTED METHODS
//
// ************************************************************************


static bool alloc_error;
static XErrorHandler oldhandler;

static int catch_alloc (Display *dpy, XErrorEvent *err)
{
   if (err->error_code == BadAlloc) {
      alloc_error = true;
   }
   if (alloc_error) return 0;
   return oldhandler(dpy, err);
}

void KPSWidget::setup()
{    
    kdDebug(4500) << "KPSWidget::setup()" << endl;
    
    int newWidth = 0, newHeight = 0;
    switch( _orientation ) {
    case KGV::PORTRAIT:
	newWidth = (int) (_pageSize.width() / 72.0 * _xdpi + 0.5);
	newHeight = (int) (_pageSize.height() / 72.0 * _ydpi + 0.5);
	break;
    case KGV::LANDSCAPE:
	newWidth = (int) (_pageSize.height() / 72.0 * _xdpi + 0.5);
	newHeight = (int) (_pageSize.width() / 72.0 * _ydpi + 0.5);
	break;
    case KGV::UPSIDEDOWN:
	newWidth = (int) (_pageSize.width() / 72.0 * _xdpi + 0.5);
	newHeight = (int) (_pageSize.height() / 72.0 * _ydpi + 0.5);
	break;
    case KGV::SEASCAPE:
	newWidth = (int) (_pageSize.height() / 72.0 * _xdpi + 0.5);
	newHeight = (int) (_pageSize.width() / 72.0 * _ydpi + 0.5);
	break;
    }
    bool sizeChanged = newWidth != width() || newHeight != height();
    setFixedSize( newWidth, newHeight );

    kapp->processEvents();

    stopInterpreter();

    // If the size of the widget changed, a new _backgroundPixmap should be 
    // created. Therefore we have to delete it first.
    if( sizeChanged && _backgroundPixmap != None ) {
	XFreePixmap( gs_display, _backgroundPixmap );
	_backgroundPixmap = None;
	XSetWindowBackgroundPixmap( gs_display, gs_window, None );
    }

    if( intConfig->backingStoreType() == KGVConfigDialog::PIX_BACKING ) {
	if( _backgroundPixmap == None ) {
	    XSync( gs_display, false );
	    oldhandler = XSetErrorHandler( catch_alloc );
	    alloc_error = false;
	    _backgroundPixmap = XCreatePixmap(
                    gs_display, gs_window,
                    width(), height(),
                    DefaultDepth( gs_display, DefaultScreen( gs_display ) ) );
	    XSync( gs_display, false );
	    if( alloc_error ) {
		if( _backgroundPixmap != None ) {
		    XFreePixmap( gs_display, _backgroundPixmap );
		    XSync( gs_display, false );
		    _backgroundPixmap = None;
		}
	    }
	    oldhandler = XSetErrorHandler( oldhandler );
	    if( _backgroundPixmap != None ) {    
		GC gc;
		XGCValues values;

		values.foreground = WhitePixel( gs_display, 
					DefaultScreen( gs_display) );
		values.background = BlackPixel( gs_display, 
					DefaultScreen( gs_display) );

		gc = XCreateGC( gs_display,
			RootWindow( gs_display, DefaultScreen( gs_display ) ),
			( GCForeground | GCBackground ), &values );

		if( _backgroundPixmap != None ) {
			XFillRectangle( gs_display, _backgroundPixmap,
			gc, 0, 0, width(), height() );
		}

		XSetWindowBackgroundPixmap( gs_display, gs_window,
					    _backgroundPixmap);
	    }
	}
    }
    else {
	if( _backgroundPixmap != None ) {
	    XFreePixmap( gs_display, _backgroundPixmap );
	    _backgroundPixmap = None;
	    XSetWindowBackgroundPixmap( gs_display, gs_window, None );
	}
    }
	    
    XSetWindowAttributes xswa;
    if( _backgroundPixmap != None ) {
	xswa.backing_store = NotUseful;
	XChangeWindowAttributes( gs_display, gs_window,
				 CWBackingStore, &xswa );
    }
    else {
	xswa.backing_store = Always;
	XChangeWindowAttributes( gs_display, gs_window,
				 CWBackingStore, &xswa );
    }

    char buf[512];

    sprintf( buf, "%ld %d %d %d %d %d %g %g %d %d %d %d",
	    _backgroundPixmap, 
	    static_cast< int >( _orientation ),
	    /*
	    _boundingBox.llx(), _boundingBox.lly(),
	    _boundingBox.urx(), _boundingBox.ury(),
	    */
	    0, 0, _pageSize.width(), _pageSize.height(),
	    _xdpi, _ydpi,
	    left_margin, bottom_margin,
	    right_margin, top_margin );
    XChangeProperty( gs_display, gs_window,
		     ghostview,
                     XA_STRING, 8, PropModeReplace,
		     (unsigned char *)buf, strlen( buf ) );

    sprintf( buf, "%s %d %d",
	    intConfig->paletteType() ==
		    KGVConfigDialog::MONO_PALETTE  ? "Monochrome" :
	    intConfig->paletteType() ==
		    KGVConfigDialog::GRAY_PALETTE  ? "Grayscale" :
	    intConfig->paletteType() ==
		    KGVConfigDialog::COLOR_PALETTE ? "Color" : "?",
	    (int) BlackPixel( gs_display, DefaultScreen( gs_display ) ),
	    (int) WhitePixel( gs_display, DefaultScreen( gs_display ) ) );
    XChangeProperty( gs_display, gs_window,
		     ghostview_colors,
		     XA_STRING, 8, PropModeReplace,
		     (unsigned char *)buf, strlen( buf ) );

    XSync( gs_display, false );  // Be sure to update properties
}


void KPSWidget::startInterpreter()
{
    GC gc;
    XGCValues values;

    values.foreground = WhitePixel(gs_display, DefaultScreen( gs_display) );
    values.background = BlackPixel(gs_display, DefaultScreen( gs_display) );

    gc = XCreateGC( gs_display,
                    RootWindow( gs_display, DefaultScreen( gs_display ) ),
                    ( GCForeground | GCBackground ), &values );

    stopInterpreter();

    // Clear the window before starting a new interpreter.
    if( _backgroundPixmap != None ) {
	XFillRectangle( gs_display, _backgroundPixmap,
			gc, 0, 0, width(), height() );
    }
    erase();

    if( _interpreterDisabled )
	return;

    _process = new KProcess;
    *_process << intConfig->interpreterPath().local8Bit();

    QStringList argList;
    if( intConfig->antiAlias() )
	argList = QStringList::split( " ", intConfig->antialiasArgs() );
    else
	argList = QStringList::split( " ", intConfig->nonAntialiasArgs() );
    
    QStringList::Iterator it;
    for( it = argList.begin(); it != argList.end(); ++it ) {
	*_process << (*it);
    }

    if( !intConfig->platformFonts() )
	*_process << "-dNOPLATFONTS";
    *_process << "-dNOPAUSE" << "-dQUIET" << "-dSAFER" << "-dPARANOIDSAFER";
    if( _usePipe )
        *_process <<
        // The following two lines are their to ensure that we are allowed to read _fileName
        "-dDELAYSAFER" << "-sInputFile="+_fileName << "-c" <<
        "<< /PermitFileReading [ InputFile ] /PermitFileWriting [] /PermitFileControl [] >> setuserparams .locksafe" <<
        "-";
    else 
	*_process << _fileName << "-c" << "quit";

    _interpreterBusy = true;
    setCursor( waitCursor );

    // WABA: Save & restore this.
    char buf[512];
    sprintf( buf, "%d", static_cast<int>( gs_window ) );
    setenv( "GHOSTVIEW", buf, true );
    setenv( "DISPLAY", XDisplayString( gs_display ), true );

    // Connect up the process.
    connect( _process, SIGNAL( processExited( KProcess* ) ),
	     this, SLOT( interpreterFailed() ) );
    connect( _process, SIGNAL( receivedStdout( KProcess*, char*, int ) ),
	     this, SLOT( gs_output( KProcess*, char*, int ) ) );
    connect( _process, SIGNAL( receivedStderr( KProcess*, char*, int ) ),
	     this, SLOT( gs_output( KProcess*, char*, int ) ) );
    connect( _process, SIGNAL( wroteStdin( KProcess*) ),
	     this, SLOT( gs_input() ) );

    kapp->flushX();

    // Finally fire up the interpreter.
    kdDebug(4500) << "KPSWidget: starting interpreter" << endl;
    if( _process->start( KProcess::NotifyOnExit, 
    		_usePipe ? KProcess::All : KProcess::AllOutput ) ) {
	_stdinReady = true;
	_interpreterReady = false;
    }
    else {
	; // TODO: Proper error handling
    }
}

void KPSWidget::stopInterpreter()
{
    // if( !_interpreterBusy ) return;
    _interpreterBusy = false;

    if( isInterpreterRunning() ) 
	_process->kill( SIGHUP );
	
    delete _process; 
    _process = 0;
    
    _inputQueue.clear();
    unsetCursor();
}

void KPSWidget::interpreterFailed()
{
    stopInterpreter();
}

/**
 * Output - receive I/O from ghostscript's stdout and stderr.
 * Pass this to the application via the output_callback. 
 */
void KPSWidget::gs_output( KProcess*, char* buffer, int len )
{
    kdDebug(4500) << "KPSWidget: received output" << endl;
    
    QString line = QString::fromLocal8Bit( buffer, len );

    if( !line.isEmpty() && intConfig->showMessages() ) {
	messages->show();
	messages->cancel->setFocus();
	messages->messageBox->append( line );
   }
}

void KPSWidget::gs_input()
{
    _stdinReady = true;

    do {
	// Close old buffer
	if( !_inputQueue.isEmpty() && bytes_left == 0 ) {
	    Record* ps_old = _inputQueue.dequeue();
	    delete ps_old;
	}

	// Open new buffer
	if( !_inputQueue.isEmpty() && _inputQueue.head()->seek_needed ) {
	    if( _inputQueue.head()->len > 0 )
		fseek( _inputQueue.head()->fp, _inputQueue.head()->begin, SEEK_SET );
	    _inputQueue.head()->seek_needed = false;
	    bytes_left = _inputQueue.head()->len;
	}
    }
    while( !_inputQueue.isEmpty() && bytes_left == 0 ); // Skip empty blocks.

    int buflen = 0;
    if( bytes_left > BUFFER_SIZE ) { 
	buflen = fread( input_buffer, sizeof (char), BUFFER_SIZE, _inputQueue.head()->fp );
    }
    else if (bytes_left > 0) {
	buflen = fread( input_buffer, sizeof (char), bytes_left, _inputQueue.head()->fp );
    }

    if( bytes_left > 0 && buflen == 0 ) {
	interpreterFailed();
	return;
    }
    bytes_left -= buflen;

    if( buflen > 0 ) {
	if( !_process->writeStdin( input_buffer, buflen ) ) {
	    interpreterFailed();
	    return;
	}
	_stdinReady = false;
    } 
    else {
	_interpreterReady = true;
    }
}

bool KPSWidget::x11Event( XEvent* e )
{
    if( e->xany.type == ClientMessage ) {
	_mWin = e->xclient.data.l[0];

	XClientMessageEvent *cme = ( XClientMessageEvent* ) e;

	if( cme->message_type == page ) {
	    kdDebug(4500) << "KPSWidget: received PAGE" << endl;
	    _interpreterBusy = false;
	    unsetCursor();
	    return true;
	}
	else if( cme->message_type == done ) {
	    kdDebug(4500) << "KPSWidget: received DONE" << endl;
	    disableInterpreter();
	    return true;
	}
    }
    return QWidget::x11Event( e );
}


