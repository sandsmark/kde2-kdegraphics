#ifndef __KGV_VIEW_H
#define __KGV_VIEW_H

#include <kparts/browserextension.h>
#include <kparts/factory.h>

class QFrame;
class QWidget;

class KAction;
class KActionCollection;
class KDirWatch;
class KInstance;
class KToggleAction;

class KGVMiniWidget;
class KGVBrowserExtension;
class KGVPageView;
class MarkList;
class ScrollBox;

class KGVFactory : public KParts::Factory
{
  Q_OBJECT
public:
  KGVFactory();
  virtual ~KGVFactory();

  virtual KParts::Part *createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *classname, const QStringList &args );

  static KInstance *instance();

private:
  static KInstance *s_instance;
};


class KSelectAction;

class KGVPart: public KParts::ReadOnlyPart
{
  Q_OBJECT
public:
    KGVPart( bool bBrowserView, QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name );
    virtual ~KGVPart();

    KGVMiniWidget *miniWidget() const { return _docManager; }
    MarkList* markList() const { return _markList; }
    KGVPageView* pageView() const { return _pageView; }

    bool openStdin();

    /**
     * Reimplemented from ReadOnlyPart in order to delete the file from 
     * KDirWatch's list.
     */
    bool closeURL();

public slots:
    void slotScrollLeft();   
    void slotScrollRight();
    void slotScrollUp();
    void slotScrollDown();
    void slotReadDown();
    void slotReadUp();
    void slotPrevPage();
    void slotNextPage();
    void slotGotoStart();
    void slotGotoEnd();
    
    void slotShowScrollBars ();
    void slotShowMarkList ();
    void slotCancelWatch();

protected slots:
    void slotFancy();
    void slotFileDirty( const QString& );
    void slotOrientation (int);
    void slotMedia (int);
    void slotNewPage( int );
    void slotPageMoved( int, int );
    void slotWatchFile();

protected:
    KToggleAction *showscrollbars, *watchaction, *fancyAct, *showmarklist;
    KSelectAction *orientation, *media;
    KActionCollection *statedep;

    // reimplemented from ReadOnlyPart
    virtual bool openFile();
    
    void enableStateDepActions( bool enable );
    void updatePageDepActions();
    void updateReadUpDownActions();

    void readSettings();
    void writeSettings();

private:
    KGVBrowserExtension* _extension;

    QWidget*	    _mainWidget;
    KGVPageView*    _pageView;
    ScrollBox*	    _scrollBox;
    QFrame*	    _divider;
    MarkList*	    _markList;
    KGVMiniWidget*  _docManager;

    KAction* zoomInAct;
    KAction* zoomOutAct; 
    KAction* backAct;
    KAction* forwardAct;
    KAction* startAct; 
    KAction* endAct; 
    KAction* readUpAct; 
    KAction* readDownAct; 
    KAction* gotoAct;
  
    KDirWatch* _watch;
};

class KGVBrowserExtension : public KParts::BrowserExtension
{
  Q_OBJECT
  friend class KGVPart; // emits our signals
public:
  KGVBrowserExtension( KGVPart *parent );
  virtual ~KGVBrowserExtension() {}

public slots:
  // Automatically detected by konqueror
  void print();

};

#endif


