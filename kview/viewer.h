/*
 * viewer.h -- Declaration of class KImageViewer.
* Generated by newclass on Wed Oct 15 11:37:16 EST 1997.
*/
#ifndef SSK_VIEWER_H
#define SSK_VIEWER_H

#include <kmainwindow.h>
#include <kaccel.h>
#include <qwmatrix.h>
#include <kurl.h>

class QMenuData;
class QPopupMenu;
class QMouseEvent;
class QString;
class QTimer;
class QAccel;

class KAccel;
class KDNDDropZone;
class KFiltMenuFactory;
class KConfigGroup;
class KImageCanvas;
class KImageFilter;
class KLocale;
class KMenuBar;
class KStatusBar;
class KIOJob;
class QDropEvent;
class KRecentFilesAction;
class KAction;
class KToggleAction;
class KActionMenu;
class KFilterAction;
class KFilterList;
class KWinModule;

class ImageListDialog;
class KAccelMenuWatch;
class InfoWin;


/**
* Top-level window
* @author Sirtaj Singh Kang (taj@kde.org)
* @version $Id: viewer.h 110000 2001-08-10 15:01:12Z mkretz $
*/
class KImageViewer : public KMainWindow
{
  Q_OBJECT
public:
  /**
  * KImageViewer Constructor
  */
  KImageViewer( KFilterList* );

  /**
  * KImageViewer Destructor
  */
  virtual ~KImageViewer();

  /**
  * Release the current cut buffer, if any.
  */
  void freeCutBuffer();

  /**
  * create a new tempfile where we can put this image
  */
  void addImage( const QImage& );

signals:
  void wantHelp( const QString & tag );

  /**
  * User has requested a new viewer object.
  */
  void wantNewViewer();

  /**
  * Viewer has lost the will to live.
  */
  void wantToDie( KImageViewer * );
  
  /**
  * A new image has been placed in the clipboard.
  */
  void newCutBuffer( QPixmap *buffer );

public slots:
  // File operations
  /** */
  void slot_load();
  /** */
  void slot_openRecent( const KURL& );
  /** */
  void slot_fileClose();

  /** */
  void slot_quitApp();

  // Size
  /** */
  void slot_zoomIn10();
  /** */
  void slot_zoomOut10();
  /** */
  void slot_zoomIn200();
  /** */
  void slot_zoomOut50();
  /** */
  void slot_zoomCustom();

  // Desktop
  void slot_tile();
  void slot_max();
  void slot_maxpect();

  // Rotate
  void slot_rotate90();
  void slot_rotate180();
  void slot_rotate270();

  // Flip
  void slot_flipVertical();
  void slot_flipHorizontal();

  void slot_reset();

  /** */
  void slot_prefs();

  /**
   * Opens the Keys Configuration Dialog
   */
  void slot_confAccels();

  /**
  * Queues into the view list.
  */
  void slot_appendURL( const KURL &url, bool show = false );

  /**
  * Toggle full screen mode.
  */
  void slot_fullScreen();

  /**
  * Ask for print settings and print the image.
  */
  void slot_printImage();

  void slot_cut();
  void slot_copy();
  void slot_paste();

  void slot_contextPress( const QPoint& );


  void slot_toggleMenuBar();
  void slot_toggleToolBar();
  void slot_toggleStatusBar();

  void slot_firstImage();

protected slots:
  void slot_newViewer();
  void slot_closeViewer();
  void slot_editToolbars();
  void slot_newToolbarConfig();
  void slot_saveOptions();
  /** 
   * Save the current image on disk 
   */
  void slot_save();
  /**
   * Ask for a filename and save on disk
   */
  void slot_saveAs();

private slots:
  void slot_invokeFilter( KImageFilter *filter );

  void slot_setStatus( const QString & status );
  void slot_message( const QString & message );
  //void setProgress( int pct );

  void slot_toggleImageList();

  void slot_loadFile( const QString & file, const KURL &url = KURL() );
  
  /**
   * The toolbar moved - either change the size of the window or the size
   * of the image
   */
  void slot_toolBarMoved();
  void slot_toolBarOrientationChanged();

  /**
   * Opens a window to show informations about the image
   */
  void slot_showImageInfo();

  /**
   * crops the image
   */
  void slot_cropImage();

  // Zoom menu functions
  void slot_zoom50();
  void slot_zoom100();
  void slot_zoom200();
  void slot_zoomMaxpect();
  void slot_zoomMax();

protected:

  /**
  * This should be called _before_ show!
  */
  void connectFilters( KFilterList* );

  /** save the image to a file */
  bool saveFile( const QString&, QString, const KURL& );
  /** uses KAction - this function does the setup */
  void setupActions( KFilterList* );  
  /** enables/disables some menu entries */
  void menuEntriesEnabled( bool );

  virtual void closeEvent( QCloseEvent * );

  virtual void saveProperties( KConfig* );
  virtual void readProperties( KConfig* );

  virtual void restoreOptions();

  virtual void dragEnterEvent(QDragEnterEvent* event);
  virtual void dropEvent(QDropEvent *e);

  virtual void resizeEvent( QResizeEvent * );

  virtual void keyPressEvent( QKeyEvent * );

private:
  /** image canvas */
  KImageCanvas  *_canvas;
  bool    _imageLoaded;
    
  uint m_bFullScreen:1;

  KRecentFilesAction *m_recent;
  KActionMenu *m_rotateMenu;
  KActionMenu *m_flipMenu;
  KActionMenu *m_desktopMenu;
  KActionMenu *m_zoomMenu;
  KAction *m_zoom50;
  KAction *m_zoom100;
  KAction *m_zoom200;
  KAction *m_zoomMaxpect;
  KAction *m_zoomMax;
  KAction *m_fileSave;
  KAction *m_fileSaveAs;
  KAction *m_fileClose;
  KAction *m_filePrint;
  KAction *m_editCrop;
  KAction *m_editReset;
  KAction *m_viewZoom;
  KAction *m_viewZoomIn;
  KAction *m_viewZoomOut;
  KAction *m_viewDouble;
  KAction *m_viewHalf;
  KAction *m_viewFullScreen;
  KAction *m_rotate90;
  KAction *m_rotate180;
  KAction *m_rotate270;
  KAction *m_flipV;
  KAction *m_flipH;
  KAction *m_desktopTile;
  KAction *m_desktopMax;
  KAction *m_desktopMaxpect;
  KAction *m_imageList;
  KAction *m_imagePrev;
  KAction *m_imageNext;
  KAction *m_imageFirst;
  KAction *m_imageLast;
  KAction *m_imageSlide;
  KAction *m_imageInfo;
  KFilterAction *m_filterMenu;
  KToggleAction *m_settingsShowMenuBar;
  KToggleAction *m_settingsShowToolBar;
  KToggleAction *m_settingsShowStatusBar;

  QString m_filename;
  QString m_format;
  KURL m_url;

  QString   *_pctBuffer;
  int    _lastPct;

  QTimer    *_msgTimer;

  ImageListDialog  *_imageList;

  QPoint    _posSave;
  QSize    _sizeSave;
  QWMatrix  _mat;

  //int    _zoomFactor;

  enum LoadMode { ResizeNone = 0, ResizeWindow = 1, ResizeImage = 2, FastScale = 4 };
  int m_resizeMode;
  KAccel *m_accel;

  /**
   * Infos about the WorkArea
   */
  KWinModule *m_kwinmodule;

  /**
   * Image infos
   */
  InfoWin *m_imageinfo;

  bool m_toolbarorientationchanged;

private:
  /**
   * save the configuration
   */
  void saveConfiguration();

  /**
  * Resize the window to fit the image.
  */
  void rzWinToImg();

  /**
   * add/subtract the width and height of the menubar, toolbar and statusbar
   * as apropiate
   */
  void sizeCorrection( int &, int &, bool add = true );

  /**
   * zoom the image as wanted, but take care if the user wants fast scaling
   */
  void zoom( double );
};

#endif // SSK_VIEWER_H
