// $Id: kpaint.cpp 107884 2001-07-26 22:50:53Z rich $

#include <qclipboard.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qwidget.h>
#include <qsignalmapper.h>
#include <config.h>

#include <kaccel.h>
#include <kaction.h>
#include <kapp.h>
#include <kcolorbtn.h>
#include <kcolorcombo.h>
#include <kcolordlg.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kedittoolbar.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kimageio.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmainwindow.h>
#include <kstdaccel.h>
#include <kstdaction.h>
#include <ktoolbar.h>
#include <kurl.h>

#include "canvas.h"
#include "version.h"
#include "kpaint.h"
#include "manager.h"
#include "canvassize.h"
#include "palettedialog.h"
#include "infodialog.h"
#include "depthdialog.h"
#include "mainview.h"
#include "navwidget.h"

KPaint::KPaint(const KURL & _url) : KMainWindow(0)
{
    tmpFile = 0;
    zoom= 100;

    mv = new MainView(this);
    v = mv->getViewport();
    c = mv->getCanvas();

    v->resize(c->size());
    setCentralWidget( mv );

    man= new Manager(c);

    initMenus();
    initSideBar();
    initStatus();
    readOptions();

    enableEditPaste();
    connect( kapp->clipboard(), SIGNAL( dataChanged() ), SLOT( enableEditPaste() ) );
    resize(640,480);
    show();
    c->clearModified();
    
    if ( !_url.isEmpty() )
        loadFile(_url);
    else
        fileNew();

    allowEditPalette = c->pixmap()->depth() == 8;
    updateCommands();
    connect(c, SIGNAL(modified()), this, SLOT(updateCommands()));
}

KPaint::~KPaint()
{
    delete man;
    delete tmpFile;
}


void KPaint::setPixmap(QPixmap *p)
{
    url = KURL();
    c->setPixmap(p);
}


bool KPaint::queryClose()
{
  if (!c->isModified())
    return true;

  QString msg = i18n(""
        "The image has been modified.\n"
        "Would you like to save it?" );

  switch( KMessageBox::warningYesNoCancel( this, msg ) )
  {
     case KMessageBox::Yes: // Save, then exit
          if( url.isEmpty())
          {
             if (!fileSaveAs())
                 return false;
          }
          else
          {
             if (!fileSave())
                 return false;
          }
          return true;

     case KMessageBox::No: // Don't save but exit.
          return true;

     case KMessageBox::Cancel: // Don't save and don't exit.
     default:
	  break; // Don't exit...
  }
  return false;

}

void KPaint::updateCommands()
{
    revertAction->setEnabled(c->isModified());
    saveAction->setEnabled(c->isModified());
    saveAsAction->setEnabled(c->isModified());
    paletteAction->setEnabled(allowEditPalette);
    setCaption(filename, c->isModified());
}

void KPaint::readOptions()
{
    KConfig *config = KApplication::kApplication()->config();

    config->setGroup( "Appearance" );

    // Read the entries
    toolbarAction->setChecked(config->readBoolEntry("ShowToolsToolBar", true));
    statusbarAction->setChecked(config->readBoolEntry("ShowStatusBar", true));

    updateControls();
}


void KPaint::writeOptions()
{
    KConfig *config = KApplication::kApplication()->config();

    config->setGroup( "Appearance" );

    // Write the entries
    config->writeEntry("ShowToolsToolBar", toolbarAction->isChecked());
    config->writeEntry("ShowStatusBar", statusbarAction->isChecked());
    config->sync();
}


void KPaint::enableEditPaste()
{
    QClipboard *cb = kapp->clipboard();
    QPixmap clipPix = cb->pixmap();
    bool e = !clipPix.isNull();
    kdDebug() << "kpaint: clipboard has image: " << e << endl;
    pasteAction->setEnabled( e );
    pasteImageAction->setEnabled( e );
}


void KPaint::updateControls()
{
    KToolBar *bar = toolBar("mainToolBar");
    if (bar)
    {
        if (toolbarAction->isChecked())
            bar->show();
        else
            bar->hide();
    }

    bar = toolBar("sideToolBar");
    if (bar)
    {
        if (toolbarAction->isChecked())
            bar->show();
        else
            bar->hide();
    }

    if (statusbarAction->isChecked())
        statusbar->show();
    else
        statusbar->hide();
}


void KPaint::canvasSizeChanged()
{
    QString size("%1 x %2");
    statusbar->changeItem(size.arg( c->width() ).arg( c->height() ), ID_FILE_SIZE);
    mv->updateLayout();
}


void KPaint::initStatus()
{
    statusbar= statusBar();

    /* write the image size */
    QString size( "%1 x %2" );
    statusbar->insertItem( size.arg( c->width() ).arg( c->height() ), ID_FILE_SIZE );

    /* write the color depth */
    size.sprintf(" %d bpp", c->getDepth());
    statusbar->insertItem(size, ID_COLOR_DEPTH);

    /* write the zoomfactor */
    statusbar->insertItem("100%", ID_ZOOM_FACTOR);

    /* write the toolname */
    /* get the max. font length of the toolnames */
    ToolList tl = man->getToolList();
    QFontMetrics fm = statusbar->fontMetrics();
    Tool *maxt = 0;
    uint maxtlen= 0, tmp;

    for ( Tool *t= tl.first(); t; t= tl.next() )
    {
        tmp = fm.width(t->getName());
        /* check the tool with the max. name len */
        if ( maxtlen < tmp )
        {
            maxtlen = tmp;
            maxt = t;
        }
    }
    /* write the maxlen toolname */
    if (maxt)
      size = maxt->getName();
    else
      size = "XXXX";
    statusbar->insertItem(size, ID_TOOL_NAME);

    /* write the filename */
    statusbar->insertItem(url.path(), ID_FILE_NAME);

    /* update to the current toolname */
    statusbar->changeItem(man->getCurrentTool().getName(), ID_TOOL_NAME);
    man->setStatusBar(statusbar);

    statusbar->show();
}


void KPaint::slotSideToolbarMoved()
{
    kdDebug() << "Sidebar moved" << endl;
}

/*
    this sidebar is actually a toolbar with custom widgets inserted
    instead of the usual buttons.  It can be docked to either side,
    top or bottom of window but starts on right side
*/
bool KPaint::initSideBar()
{
    int iw = 66;
    int ih = 66;

    // the toolbar itself
    sideToolbar = toolBar("sideToolBar");
    sideToolbar->show();
    sideToolbar->setBarPos(KToolBar::Right);

    connect(sideToolbar, SIGNAL(orientationChanged(Orientation)),
	    this, SLOT(slotSideToolbarMoved()));

    // the preview/navigation widget with container
    QWidget *navWidget = new QWidget(sideToolbar);

    navWidget->setMinimumWidth(iw);
    navWidget->setMinimumHeight(ih);
    navWidget->setMaximumWidth(120);
    navWidget->setMaximumHeight(120);

    QBoxLayout *ml = new QVBoxLayout(navWidget);
    //QLabel *navlabel = new QLabel(i18n("Image Preview"), navWidget);

    nav = new NavWidget(navWidget, "navwidget");
    nav->setMinimumSize(120, 80);
    nav->setMaximumSize(120, 120);
    nav->setPixmap(c->pixmap());

    connect(c, SIGNAL(pixmapChanged(QPixmap *)),
	    nav, SLOT(setPixmap(QPixmap *)));
    connect(c, SIGNAL(modified()),
        nav, SLOT(pixmapChanged()));
    //ml->addWidget(navlabel);
    ml->addWidget(nav);

    QWidget *mouseWidget = new QWidget(sideToolbar);
    mouseWidget->setFixedSize(66, 96);
    QBoxLayout *vl = new QVBoxLayout(mouseWidget);

    // The colour selector widgets with container widget
    QWidget *colorsWidget = new QWidget(mouseWidget);

    colorsWidget->setMinimumWidth(32);
    colorsWidget->setMinimumHeight(32);
    colorsWidget->setMaximumWidth(66);
    colorsWidget->setMaximumHeight(66);

    ml = new QHBoxLayout(colorsWidget);
    //QLabel *lmbLabel = new QLabel(i18n("LeftButton Color"), colorsWidget);
    //QLabel *rmbLabel = new QLabel(i18n("RightButton Color"), colorsWidget);
    lmbColourBtn = new KColorButton(red, colorsWidget, "lmbColour");
    rmbColourBtn = new KColorButton(green, colorsWidget, "rmbColour");
    //    QPushButton *mmbBtn = new QPushButton(colorsWidget, "mmbButton");

    lmbColourBtn->setFixedSize(22, 30);
    //    mmbBtn->setFixedSize(22, 30);
    rmbColourBtn->setFixedSize(22, 30);

    man->setLMColorButton(lmbColourBtn);
    man->setRMColorButton(rmbColourBtn);

    connect(lmbColourBtn, SIGNAL(changed(const QColor &)),
	    man, SLOT(setLMBcolour(const QColor &)));
    connect(rmbColourBtn, SIGNAL(changed(const QColor &)),
	    man, SLOT(setRMBcolour(const QColor &)));

    //ml->addWidget(rmbLabel);
    ml->addWidget(lmbColourBtn);
    //    ml->addWidget(mmbBtn);
    ml->addWidget(rmbColourBtn);
    //ml->addWidget(lmbLabel);

    vl->addWidget(colorsWidget);

    //    QWidget *mouseBodyWidget = new QWidget(mouseWidget);
    //    ml = new QHBoxLayout(mouseBodyWidget);
    //    QPushButton *mouseBtn = new QPushButton(mouseBodyWidget, "MouseButton");
    //    mouseBtn->setFixedSize(66, 60);
    //    ml->addWidget(mouseBtn);
    //    vl->addWidget(mouseBodyWidget);


    KPaletteTable *paletteTable = new KPaletteTable( sideToolbar , 120 , 8 );
    paletteTable->setMaximumHeight(66);

    connect(paletteTable, SIGNAL(colorSelected(const QColor &, const QString &)),
	  man, SLOT(setLMBcolour(const QColor &)));

    // insert containter widgets so they show up as toolbar items
    sideToolbar->insertWidget(ID_NAV_WIDGET, iw, navWidget);
    sideToolbar->insertSeparator();
    sideToolbar->insertWidget(ID_COLORS_WIDGET, iw, mouseWidget);
    sideToolbar->insertSeparator();
    sideToolbar->insertWidget(ID_COLOR_BAR, iw, paletteTable);

    return true;
}


void KPaint::initMenus()
{
    // file menu
    (void) KStdAction::openNew(this, SLOT(fileNew()), actionCollection());
    (void) KStdAction::open(this, SLOT(fileOpen()), actionCollection());
    (void) KStdAction::close(this, SLOT(fileClose()), actionCollection());
    revertAction = KStdAction::revert(this, SLOT(fileRevert()), actionCollection());
    saveAction = KStdAction::save(this, SLOT(fileSave()), actionCollection());
    saveAsAction = KStdAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
    (void) KStdAction::quit(this, SLOT(close()), actionCollection());

    // edit menu
    copyAction = KStdAction::copy(this, SLOT(editCopy()), actionCollection());
    cutAction = KStdAction::cut(this, SLOT(editCut()), actionCollection());
    pasteAction = KStdAction::paste(this, SLOT(editPaste()), actionCollection());
    pasteImageAction = new KAction(i18n("Paste &image"), 0, this,
                       SLOT(editPasteImage()), actionCollection(),
                       "edit_paste_image");

    // view menu
    zoomInAction = KStdAction::zoomIn(this, SLOT(editZoomIn()), actionCollection());
    zoomOutAction = KStdAction::zoomOut(this, SLOT(editZoomOut()), actionCollection());
    zoomNormalAction = KStdAction::actualSize(this, SLOT(editZoomNormal()), actionCollection());
    zoomAction = new KSelectAction( i18n( "Zoom" ), 0, actionCollection(), "view_zoom" );
    connect( zoomAction, SIGNAL( activated( const QString & ) ),
             this, SLOT( viewZoom( const QString & ) ) );
    QStringList lst;
    lst << "10%";
    lst << "20%";
    lst << "30%";
    lst << "40%";
    lst << "50%";
    lst << "60%";
    lst << "70%";
    lst << "80%";
    lst << "90%";
    lst << "100%";
    lst << "200%";
    lst << "300%";
    lst << "400%";
    lst << "500%";
    lst << "600%";
    lst << "700%";
    lst << "800%";
    lst << "900%";
    lst << "1000%";
    zoomAction ->setItems( lst );
    zoomAction ->setCurrentItem(9);
    // image menu
    (void) new KAction(i18n("&Information..."), 0, this,
                       SLOT(imageInfo()), actionCollection(),
                       "image_info");
    (void) new KAction(i18n("&Resize..."), 0, this,
                       SLOT(imageResize()), actionCollection(),
                       "image_resize");
    paletteAction = new KAction(i18n("&Edit palette..."), 0, this,
                       SLOT(imageEditPalette()), actionCollection(),
                       "image_palette");
    (void) new KAction(i18n("&Change color depth..."), 0, this,
                       SLOT(imageChangeDepth()), actionCollection(),
                       "image_color");

    // settings menu
    toolbarAction = KStdAction::showToolbar(this, SLOT(updateControls()), actionCollection());
    statusbarAction = KStdAction::showStatusbar(this, SLOT(updateControls()), actionCollection());
    (void) KStdAction::saveOptions(this, SLOT(writeOptions()), actionCollection());

#if 0
    // window menu
    (void) new KAction(i18n("&New window"), 0, this,
                       SLOT(newWindow()), actionCollection(),
                       "window_new");
    (void) new KAction(i18n("&Close window"), 0, this,
                       SLOT(closeWindow()), actionCollection(),
                       "window_close");
#endif

    // tool menu
    (void) new KAction(i18n("&Tool properties..."), 0, this,
                   SLOT(toolProperties()), actionCollection(),
                   "tool_config");

    QSignalMapper *mapper = new QSignalMapper(this);
    connect( mapper, SIGNAL( mapped(int) ),
	     this, SLOT( setTool(int) ) );

    int id = 0;
    ToolList tl = man->getToolList();
    for ( Tool *t= tl.first(); t; t= tl.next() ) 
    {
      if ( !t->isSpecial() ) {
	KAction *action = new KAction(t->getName(), QIconSet(t->pixmap()), 0, 
				      0, 0,
				      actionCollection(),
				      QCString().sprintf("tool_%d", id));

	mapper->setMapping( action, id++ );
	connect( action, SIGNAL( activated() ),
		 mapper, SLOT( map() ) );
      }
    }

    KStdAction::configureToolbars(this, SLOT(editToolbars()), actionCollection());
    createGUI("kpaint.rc");
}


void KPaint::editToolbars()
{
  KEditToolbar dlg(actionCollection(),"kpaint.rc");
  if (dlg.exec())
    createGUI("kpaint.rc");
}


bool KPaint::fileOpen()
{
    bool result = false;
    if (!url.isEmpty() || c->isModified())
    {
        KPaint *kp = new KPaint();
        result = kp->fileOpen();
        if (result)
           kp->show();
        else
           delete kp;
        return result;
    }

    kdDebug(4400) << "fileOpen()" << endl;

    KURL url = KFileDialog::getImageOpenURL(QString::null, this);

    if (url.isEmpty())
        return false;

    return loadFile(url);
}


bool KPaint::loadFile(const KURL& _url)
{
    bool result = false;
    KTempFile *newTmpFile = 0;

    if (_url.isLocalFile())
    {
       result = c->load(_url.path());
       if (result)
          filename = _url.path();
    }
    else
    {
       newTmpFile = new KTempFile();
       newTmpFile->setAutoDelete(true);
       newTmpFile->close();
       QString tmpname = newTmpFile->name();
       if (KIO::NetAccess::download(_url, tmpname))
       {
          result =  c->load(tmpname);
       }
       if (result)
          filename = tmpname;
       else {
          delete newTmpFile;
          newTmpFile = 0L;
       }
    }

    if (!result)
    {
       QString msg
                = i18n("KPaint could not open %1 \n").arg(_url.prettyURL());
       KMessageBox::sorry(this, msg);
       return false;
    }

    delete tmpFile;
    tmpFile = newTmpFile;
    url = _url;
    format = KImageIO::type(url.fileName());
    statusbar->changeItem(url.prettyURL(), ID_FILE_NAME);
    orig_width = c->width();
    orig_height = c->height();
    canvasSizeChanged();
    c->clearModified();
    updateCommands();
    return true;
}


bool KPaint::fileSave()
{
    kdDebug(4400) << "fileSave()\n" << endl;

    if(url.path().isEmpty()) {
        kdDebug(4400) << "Path '" << url.path() << "' is empty\n" << endl;

        if (!fileSaveAs()) {
           kdDebug(4400) << "Passing to save as\n" << endl;
           return false;
	}
    }
    else if (!saveFile(filename, format, url))
      return false;

    return true;
}


bool KPaint::saveFile(QString filename_, QString format_, KURL &url_)
{
    kdDebug(4400) << "saveFile reached, calling canvas save...\n" << endl;

    if (!c->save(filename_, format_.ascii()))
        return false;

    filename = filename_;
    format = format_;

    if (!url_.isLocalFile())
    {
        if (!KIO::NetAccess::upload(filename_, url_))
	    return false;
    }
    filename = filename_;
    format = format_;
    url = url_;

    statusbar->changeItem(filename, ID_FILE_NAME);
    c->clearModified();
    updateCommands();
    return true;
}


bool KPaint::fileSaveAs()
{
    kdDebug(4400) << "fileSaveAsCommand" << endl;

    KURL file = KFileDialog::getSaveURL(QString::null, KImageIO::pattern(KImageIO::Writing), this);

    if(file.isEmpty())
        return false;

    QString tmpname;

    KTempFile *newTmpFile = 0;
    if (file.isLocalFile())
    {
        tmpname = file.path();
    }
    else
    {
        newTmpFile = new KTempFile();
        newTmpFile->setAutoDelete(true);
        newTmpFile->close();
        tmpname = newTmpFile->name();
    }

    if (!saveFile(tmpname, KImageIO::type(file.path()).ascii(), file))
    {
        delete newTmpFile;
        return false;
    }
    delete tmpFile;
    tmpFile = newTmpFile;

    c->clearModified();
    return true;
}

void KPaint::fileClose()
{
    delete tmpFile;
    tmpFile = 0;
    url = KURL();
    c->clearModified();
    fileNew();
}

void KPaint::fileRevert()
{
    if (!url.isEmpty())
    {
       c->load(filename);
    }
    else
    {
       QPixmap p;
       p.resize(orig_width, orig_height);
       p.fill(QColor("white"));
       c->setPixmap(&p);
    }
    canvasSizeChanged();
    c->clearModified();
    updateCommands();
}

void KPaint::fileNew()
{
    kdDebug(4400) << "File New" << endl;

    if (!url.isEmpty() || c->isModified())
    {
        KPaint *kp = new KPaint();
        kp->show();
        kp->fileNew();
        return;
    }

    c->clearModified();
    QPixmap p;
    p.resize(320, 200);
    p.fill(QColor("white"));
    c->setPixmap(&p);
    man->setCurrentTool(0);
    format = "PNG";
    filename = i18n("untitled") + ".png";
    url=KURL();
    delete tmpFile;
    tmpFile = 0;

    statusbar->changeItem(filename, ID_FILE_NAME);
    orig_width = c->width();
    orig_height = c->height();
    canvasSizeChanged();
    repaint(0);

//      canvasSizeDialog sz(this, "canvassizedlg",c->width(), c->height());
//      if ( sz.exec() ) {
//          p.resize( sz.getWidth(), sz.getHeight() );
//          p.fill( QColor("white") );
//          c->setPixmap(&p);
//          canvasSizeChanged();
//          repaint(0);
//      }
}

// Edit
void KPaint::editCopy()
{
    kdDebug(4400) << "editCopy()\n" << endl;
    c->copy();
}

void KPaint::editCut()
{
    kdDebug(4400) << "editCut()\n" << endl;
    c->cut();
}

/*
    paste is a tool now. It's in brush.cpp and brush.h,
    and can later be extended for painting with clip
    pixmap as a brush as well as pasting down
*/
void KPaint::editPaste()
{
    kdDebug(4400) << "editPaste()\n" << endl;

    QClipboard *cb = kapp->clipboard();
    QPixmap clipPix = cb->pixmap();
    if ( !clipPix.isNull() )
      man->setCurrentTool(9);
}

/* This is intended to copy clipboard to a
   file and open it in a new window - */

void KPaint::editPasteImage()
{
    kdDebug(4400) << "editPasteImage()" << endl;

    QClipboard *cb = kapp->clipboard();
    QPixmap clipPix = cb->pixmap();
    if ( !clipPix.isNull() ) {
        QPixmap *p = new QPixmap( clipPix );
        KPaint *kp = new KPaint();
        kp->setPixmap(p);
	kp->c->markModified();
        kp->show();
    }
}

void KPaint::editZoomNormal()
{
    kdDebug(4400) << "editZoomIn()" << endl;

    int zoom = 100;
    c->setZoom( zoom );
    QString zoomstr;
    zoomstr.setNum(zoom);
    zoomstr += '%';

    statusbar->changeItem( zoomstr, ID_ZOOM_FACTOR );
    QStringList list=zoomAction ->items();
    zoomAction->setCurrentItem(list.findIndex(zoomstr));
    zoomInAction->setEnabled( true );
    zoomOutAction->setEnabled( true );
}


void KPaint::editZoomIn()
{
    kdDebug(4400) << "editZoomIn()" << endl;

    if (zoom >= 100)
    {
        zoom += 100;
        if (zoom == 1000)
        {
            zoomInAction->setEnabled(false);
            zoomOutAction->setEnabled(true);
        }
    }
    else
    {
        zoom += 10;
        zoomInAction->setEnabled(true);
        zoomOutAction->setEnabled(true);
    }

    c->setZoom(zoom);

    QString zoomstr;
    zoomstr.setNum(zoom);
    zoomstr += '%';

    statusbar->changeItem(zoomstr, ID_ZOOM_FACTOR);
    QStringList list=zoomAction ->items();
    zoomAction->setCurrentItem(list.findIndex(zoomstr));
}

void KPaint::viewZoom( const QString &_strZoom )
{
    QString z( _strZoom );
    z = z.replace( QRegExp( "%" ), "" );
    z = z.simplifyWhiteSpace();
    int new_zoom = z.toInt();

    if(new_zoom==10)
    {
        zoomOutAction->setEnabled(false);
        zoomInAction->setEnabled(true);
    }
    else if(new_zoom>=1000)
    {
        zoomInAction->setEnabled(false);
        zoomOutAction->setEnabled(true);
    }
    else
    {
        zoomInAction->setEnabled(true);
        zoomOutAction->setEnabled(true);
    }

    if(zoom!=new_zoom)
    {
        c->setZoom(new_zoom);
        zoom=new_zoom;
        statusbar->changeItem(_strZoom, ID_ZOOM_FACTOR);
    }
}

void KPaint::editZoomOut()
{
    kdDebug(4400) << "editZoomOut()" << endl;

    if (zoom > 100)
    {
        zoom -= 100;
        zoomOutAction->setEnabled(true);
        zoomInAction->setEnabled(true);
    }
    else
    {
        zoom -= 10;
        if (zoom <= 10)
        {
            zoomOutAction->setEnabled(false);
            zoomInAction->setEnabled(true);
        }
    }

    c->setZoom(zoom);

    QString zoomstr;
    zoomstr.setNum(zoom);
    zoomstr += '%';
    QStringList list=zoomAction ->items();
    zoomAction->setCurrentItem(list.findIndex(zoomstr));

    statusbar->changeItem(zoomstr, ID_ZOOM_FACTOR);
}


void KPaint::editOptions()
{
    kdDebug(4400) << "editOptions()" << endl;
    /* obsolet (jha)
    KKeyDialog::configureKeys(keys); */
}


void KPaint::imageInfo()
{
  imageInfoDialog info(c, 0, "Image Information");
  kdDebug(4400) << "imageInfo()" << endl;
  info.exec();
}


void KPaint::imageResize()
{
    kdDebug(4400) << "imageResize()" << endl;

    canvasSizeDialog sz(this,"imageResize",c->width(), c->height());
    if (sz.exec())
    {
        int w= sz.getWidth();
        int h= sz.getHeight();
        if( w!=0 && h!=0)
        {
            c->resizeImage(w, h);
            canvasSizeChanged();
        }
    }
}

void KPaint::imageEditPalette()
{
    kdDebug(4400) << "imageEditPalette()" << endl;
    paletteDialog pal(c->pixmap());

    if (pal.exec())
    {
        c->setPixmap(pal.pixmap());
        c->repaint(0);
    }
}

void KPaint::imageChangeDepth()
{
    QString depthstr;
    KStatusBar *sb = statusBar();
    depthDialog d(c);

    kdDebug(4400) << "imageChangeDepth()" << endl;

    if (d.exec())
    {
        switch (d.depthBox->currentItem())
        {

    case ID_COLOR_1:
      kdDebug(4400) << "setDepth to 1" << endl;
      c->setDepth(1);
      depthstr.sprintf(" %d bpp", 1);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= false;
      break;

    case ID_COLOR_4:
      kdDebug(4400) << "setDepth to 4" << endl;
      c->setDepth(4);
      depthstr.sprintf(" %d bpp", 4);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= false;
      break;

    case ID_COLOR_8:
      kdDebug(4400) << "setDepth to 8" << endl;
      c->setDepth(8);
      depthstr.sprintf(" %d bpp", 8);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= true;
      break;

    case ID_COLOR_15:
      kdDebug(4400) << "setDepth to 15" << endl;
      c->setDepth(15);
      depthstr.sprintf(" %d bpp", 15);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= false;
      break;

    case ID_COLOR_16:
      kdDebug(4400) << "setDepth to 16" << endl;
      c->setDepth(16);
      depthstr.sprintf(" %d bpp", 16);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= false;
      break;

    case ID_COLOR_24:
      kdDebug(4400) << "setDepth to 24" << endl;
      c->setDepth(24);
      depthstr.sprintf(" %d bpp", 24);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= false;
      break;

    case ID_COLOR_32:
      kdDebug(4400) << "setDepth to 32" << endl;
      c->setDepth(32);
      depthstr.sprintf("%d bpp", 32);
      sb->changeItem(depthstr, ID_COLOR_DEPTH);
      allowEditPalette= false;
      break;

    default:
      break;

        }
    }
}

// Tool
void KPaint::setTool(int t)
{
    man->setCurrentTool(t);
}

void KPaint::toolProperties()
{
    kdDebug(4400) << "toolProperties()" << endl;
    man->showPropertiesDialog();
}

void KPaint::dummy()
{
  // just here to make KAction happy
}

#include "kpaint.moc"
