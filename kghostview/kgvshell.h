#ifndef __KGVSHELL_H__
#define __KGVSHELL_H__

#include <qstring.h>

#include <kparts/mainwindow.h>

class QTimer;

class KRecentFilesAction;
class ScrollBox;
class KGVPart;
class KToggleAction;
class KAction;
class KActionCollection;
class KConfig;

class KGVShell : public KParts::MainWindow
{
    Q_OBJECT
	
public:
    KGVShell();
    virtual ~KGVShell();

public slots:
    void openURL( const KURL& url );
    void openStdin();

protected slots:
    void slotFileOpen();
    void slotQuit();
    void slotFitToPage();
    void slotResize();
    void slotShowToolBar();
    void slotShowStatusBar();

protected:
    // session management
    virtual void saveProperties( KConfig *config );
    virtual void readProperties( KConfig *config );
    
    void readSettings();
    void writeSettings();
    void enableStateDepActions( bool enable );

private:
    KGVPart* m_gvpart;
    QString cwd;
    KAction* openact;
    KRecentFilesAction* recent;
    KToggleAction* showtoolbar;
    KToggleAction* showstatusbar;
    KActionCollection* statedep;
    unsigned int width0, height0;
};

#endif
