#ifndef KPSWIDGET_H
#define KPSWIDGET_H

#include <kprocess.h>

#include <qfiledialog.h>
#include <stdlib.h>
#include <math.h>
#include <qapplication.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qpaintdevice.h>
#include <qpoint.h>
#include <qqueue.h>
#include <qbitmap.h>
#include <qsocketnotifier.h>
#define	 GC GC_QQQ				// avoid type mismatch
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <ctype.h>

#include "kgv.h"
#include "messages.h"
class KGVConfigDialog;

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#ifndef None
#define None -1
#endif

#ifndef XlibSpecificationRelease
typedef char *XPointer;
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef XlibSpecificationRelease
typedef char *XPointer;
#endif

#include <errno.h>
/* BSD 4.3 errno.h does not declare errno */
extern int errno;
/* Both error returns are checked for non-blocking I/O. */
/* Manufacture the other error code if only one exists. */
#if !defined(EWOULDBLOCK) && defined(EAGAIN)
#define EWOULDBLOCK EAGAIN
#endif
#if !defined(EAGAIN) && defined(EWOULDBLOCK)
#define EAGAIN EWOULDBLOCK
#endif

class KPSWidget : public QWidget
{
    Q_OBJECT

public:
    KPSWidget( QWidget *parent = 0, const char* name = 0 );
    ~KPSWidget();

    void enableInterpreter();
    void disableInterpreter();
    
    bool isInterpreterReady() const; 
    bool isInterpreterRunning() const;
    
    bool nextPage();
    bool sendPS( FILE* fp, const KGV::FileOffset&, bool close);
    
    void setFileName( const QString&, bool usePipe );

    void setPageSize( const QSize& );
    QSize pageSize() const { return _pageSize; }
    
    void setBoundingBox( const KGV::BoundingBox& );
    KGV::BoundingBox boundingBox() const { return _boundingBox; }
    
    void setOrientation( KGV::Orientation orientation );
    int orientation() const { return _orientation; }
    
    void setResolution( int xdpi, int ydpi);
    
    void layout();
    
    bool configure();
    void writeSettings();
	
protected:    
    struct Record {
	~Record() { if( close ) fclose( fp ); }
	FILE* fp;
	long begin;
	unsigned int len;
	bool seek_needed;
	bool close;
    };
    
    // void resizeEvent( QResizeEvent* );
    bool x11Event( XEvent* );
    
    void setup();
    
    void startInterpreter();
    void stopInterpreter();

protected slots:
    void gs_input();
    void gs_output(KProcess *, char *buffer, int len);
    void interpreterFailed();
	
private:
    Window _mWin;   // Destination of ghostscript messages.
    
    Display*	gs_display;
    Window	gs_window;
   
    Atom ghostview; 
    Atom ghostview_colors;
    Atom next;
    Atom page;
    Atom done;
    
    /**
     * Variables used to communicate with the gs process.
     */
    Pixmap		_backgroundPixmap;
    QSize		_pageSize;
    KGV::BoundingBox	_boundingBox;
    float   _xdpi, _ydpi;
    int	    left_margin, right_margin, bottom_margin, top_margin;

    int _baseXdpi, _baseYdpi;
    
    MessagesDialog*	messages;
    KGVConfigDialog*	intConfig;

    KProcess* _process;
 
    QQueue<Record>  _inputQueue;
    char*	    input_buffer;
    unsigned int    bytes_left;
    
    KGV::Orientation _orientation;
    bool show_messages;
    QString _fileName;
    bool _usePipe;

    bool _stdinReady;
    bool _interpreterBusy;
    bool _interpreterReady;
    bool _interpreterDisabled; // Disables starting the interpreter.
    bool _layoutDirty;
};

#endif // KPSWIDGET_H

