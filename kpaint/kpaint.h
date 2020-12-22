// -*- c++ -*-

/* modified by jha (j.habenicht@usa.net)
 */

// $Id: kpaint.h 107633 2001-07-25 15:35:16Z rich $

#ifndef KPAINT_H
#define KPAINT_H

#include <qwidget.h>
#include <qscrollview.h>
#include <kmainwindow.h>
#include <ktoolbar.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <ktempfile.h>

#include <qrect.h>
#include "canvas.h"
#include "manager.h"
#include "commands.h"
#include "navwidget.h"

#define ID_NAV_WIDGET 501
#define ID_COLORS_WIDGET 502
#define ID_COLOR_BAR 503

class MainView;
class KAction;
class KToggleAction;
class KSelectAction;
class NavWidget;
class KColorButton;
class KToolbar;

class KPaint : public KMainWindow
{
    Q_OBJECT

public:
    KPaint(const KURL & url_ = KURL());
    ~KPaint();

    void setPixmap(QPixmap *);

public slots:

    // File
    void fileNew();
    bool fileOpen();
    void fileClose();
    void fileRevert();
    bool fileSave();
    bool fileSaveAs();

    // Edit
    void editCopy();
    void editCut();
    void editPaste();
    void editPasteImage();
    void editZoomNormal();
    void editZoomIn();
    void editZoomOut();
    void editOptions();
    void editToolbars();

    //Edit Zoom
    void viewZoom( const QString & );

    // Image
    void imageInfo();
    void imageResize();
    void imageEditPalette();
    void imageChangeDepth();

    // Tool
    void toolProperties();
    void setTool(int);

    /**
    * Update toolbars/status bars etc.
    */
    void updateControls();
    void updateCommands();

    /**
    * Read the options
    */
    void readOptions();

    /**
    * Write the options
    */
    void writeOptions();

  /**
   * Update side toolbar items
   */
  void slotSideToolbarMoved();

    void enableEditPaste();

    void dummy();

protected:
    // Load/Save files
    bool loadFile(const KURL& url);
    bool saveFile(QString filename_, QString format_, KURL &url_);

    /**
    * Should we really?
    */
    bool queryClose();

private:
    // Initialisation
    void initMenus();
    void initStatus();
    bool initSideBar();

    KAction *revertAction, *copyAction, *cutAction, *pasteAction, *paletteAction,
      *saveAction, *saveAsAction, *pasteImageAction;
    KAction *zoomInAction, *zoomOutAction, *zoomNormalAction;
    KToggleAction *toolbarAction, *statusbarAction;
    KSelectAction *zoomAction;

    /**
     * Update status item
     */
    void canvasSizeChanged();

    
    // Info about the currently open image
    QString filename;          // actual local filename
    KURL url;                  // URL it came from
    QString format;            // format to use

    KTempFile *tmpFile;        // tempfile
    int zoom;

    // Command status
    bool allowEditPalette;

    // Tool manager
    Manager *man;

    // Navigation (preview) widget
    NavWidget *nav;
    // the color selection buttons    
    KColorButton *lmbColourBtn;
    KColorButton *rmbColourBtn;

    // Child widgets
    QScrollView *v;
    Canvas *c;

    KToolBar *toolsToolbar;
    KToolBar *commandsToolbar;
    KToolBar *sideToolbar;
  
    KStatusBar *statusbar;
    KMenuBar *menu;
    MainView *mv;

    QPixmap *lmbColor, *rmbColor;
    int orig_width;
    int orig_height;
};

#endif
