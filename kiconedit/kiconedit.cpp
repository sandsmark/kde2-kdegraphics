/*
    kiconedit - a small graphics drawing program for the KDE
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#include <qwhatsthis.h>
#include <qtooltip.h>

#include <kconfig.h>
#include <kruler.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktoolbarbutton.h>
#include <kdebug.h>

#include "kiconedit.h"
#include "main.h"

KIconEdit::KIconEdit(const QImage image, const char *name)
 : KMainWindow(0, name)
{
    init();
    img = image;
    img.detach();
    grid->load(&img);
    preview->setPixmap(grid->pixmap());
}

KIconEdit::KIconEdit(const QString &xpm, const char *name)
 : KMainWindow(0, name)
{
    init();
    icon->open(&img, xpm);
    preview->setPixmap(grid->pixmap());
}

void KIconEdit::init()
{
  setCaption(kapp->caption());

  printer = new KPrinter;

  kiprops = new KIconEditProperties(this);
  Properties *pprops = props(this);

  menubar = 0L;
  toolbar = 0L;
  drawtoolbar = 0L;
  palettetoolbar = 0L;
  statusbar = 0L;
  what = 0L;

  msgtimer = new QTimer(this);
  connect( msgtimer, SIGNAL(timeout()), SLOT(slotClearStatusMessage()));

  icontemplates = new KIconTemplateContainer;

  gridview = new KGridView(&img, this); 
  gridview->setShowRulers(pprops->showrulers);

  grid = gridview->grid();

  grid->setGrid(pprops->showgrid);
  grid->setCellSize(pprops->gridscaling);

  icon = new KIconEditIcon(this, &grid->image());

  setAcceptDrops(true);

  setupStatusBar();
  setupMenuBar();
  setupToolBar();
  setupDrawToolBar();
  setupPaletteToolBar();
  setupWhatsThis();

  connect( icon, SIGNAL( saved()),
           SLOT(slotSaved()));
  connect( icon, SIGNAL( loaded(QImage *)),
     grid, SLOT(load(QImage *)));
  connect( icon, SIGNAL(opennewwin(const QString &)),
           SLOT(slotNewWin(const QString &)));
  connect(icon, SIGNAL(newname(const QString &)),
           SLOT( slotUpdateStatusName(const QString &)));
  connect(icon, SIGNAL(newmessage(const QString &)),
           SLOT( slotUpdateStatusMessage(const QString &)));

  connect( syscolors, SIGNAL( newcolor(uint)),
     grid, SLOT(setColorSelection(uint)));
  connect( customcolors, SIGNAL( newcolor(uint)),
     grid, SLOT(setColorSelection(uint)));

  connect( grid, SIGNAL( changed(const QPixmap &)),
    preview, SLOT(setPixmap(const QPixmap &)));
  connect( grid, SIGNAL( addingcolor(uint) ),
           SLOT(addColor(uint)));
  connect( grid, SIGNAL( colorschanged(uint, uint*) ),
           SLOT(addColors(uint, uint*)));

  connect(grid, SIGNAL(sizechanged(int, int)),
           SLOT( slotUpdateStatusSize(int, int)));
  connect(grid, SIGNAL(poschanged(int, int)),
           SLOT( slotUpdateStatusPos(int, int)));
  connect(grid, SIGNAL(scalingchanged(int, bool)),
           SLOT( slotUpdateStatusScaling(int, bool)));
  connect( grid, SIGNAL( addingcolor(uint) ),
           SLOT(slotUpdateStatusColors(uint)));
  connect(grid, SIGNAL(colorschanged(uint, uint*)),
           SLOT( slotUpdateStatusColors(uint, uint*)));
  connect(grid, SIGNAL(newmessage(const QString &)),
           SLOT( slotUpdateStatusMessage(const QString &)));
  connect(grid, SIGNAL(clipboarddata(bool)),
           SLOT( slotUpdatePaste(bool)));
  connect(grid, SIGNAL(selecteddata(bool)),
           SLOT( slotUpdateCopy(bool)));

  kdDebug() << "Setting tool" << endl;
  slotTools(ID_DRAW_FIND);
  
  kdDebug() << "Updating statusbar" << endl;
  slotUpdateStatusSize(grid->cols(), grid->rows());
  
  slotUpdateStatusScaling(grid->scaling(), true);
  
  if(icon->url().length())
    slotUpdateStatusName(icon->url());
  
  slotUpdateCopy(false);
  
  uint *c = 0, n = 0;
  n = grid->getColors(c);
  slotUpdateStatusColors(n, c);
  
  setCentralWidget(gridview);

  if((pprops->winwidth > 0) && (pprops->winheight > 0))
    resize( pprops->winwidth, pprops->winheight );

  kdDebug() << "Showing" << endl;
  show();
}

KIconEdit::~KIconEdit()
{
    if(printer)
      delete printer;

    if(what)
      delete what;
    what = 0L;

    if(icontemplates)
      delete icontemplates;
    icontemplates = 0L;
}

bool KIconEdit::queryClose()
{
    bool cancel = false;
    if (grid->isModified()) 
    {
        int r = KMessageBox::warningYesNoCancel(this,
	    i18n("The current file has been modified.\nDo you want to save it?"));

        switch(r)
        {
            case 0:
                icon->save(&grid->image());
                break;
            case 1:
                break;
            case 2:
                cancel = true;
                break;
            default:
                break;
        }
    }
    
    if(!cancel)
    {
        writeConfig();
    }
    return (!cancel);
}

/* 
    save size of the application window
*/
void KIconEdit::resizeEvent( QResizeEvent *ev )
{
  KMainWindow::resizeEvent(ev);
  //kdDebug() << "KIconEdit::resizeEvent()" << endl;
  
  props(this)->winwidth = geometry().width();
  props(this)->winheight = geometry().height();
  
  //viewport->updateScrollBars();
}

void KIconEdit::unsaved( bool /*flag*/ )
{
    //setUnsavedData(flag);
}

// this is for exit by request of the session manager
void KIconEdit::saveProperties(KConfig *config )
{
    kdDebug() << "KIconEdit::saveProperties" << endl;
    
    config->writeEntry("Name", icon->url());
}

// this is for instances opened by the session manager
void KIconEdit::readProperties(KConfig *config)
{
    kdDebug() << "KIconEdit::readProperties" << endl;
    
    QString entry = config->readEntry("Name", ""); // no default
    if (entry.isEmpty())  return;
    icon->open(&grid->image(), entry);
}

/* 
    this is for normal exits or request from "Options->Save options".
*/
    
void KIconEdit::writeConfig()
{
    KIconEditProperties::saveProperties(this);
}

QSize KIconEdit::sizeHint() const
{
    if(gridview)
        return gridview->sizeHint();
    else
        return QSize(-1, -1);
}

bool KIconEdit::setupMenuBar()
{
  kdDebug() << "setupMenuBar" << endl;

  Properties *pprops = props(this);
  menubar = menuBar();
  pprops->menubar = menubar;

  KAccel *keys = pprops->keys; 

  keys->connectItem( KStdAccel::New, this, SLOT(slotNew()) );
  keys->connectItem( KStdAccel::Save , this, SLOT(slotSave()) );
  keys->connectItem( KStdAccel::Quit, this, SLOT(slotQuit()) );
  keys->connectItem( KStdAccel::Cut , this, SLOT(slotCut()) );
  keys->connectItem( KStdAccel::Copy , this, SLOT(slotCopy()) );
  keys->connectItem( KStdAccel::Paste , this, SLOT(slotPaste()) );
  keys->connectItem( KStdAccel::Open , this, SLOT(slotOpen()) );
  keys->connectItem( KStdAccel::Close , this, SLOT(slotClose()) );
  keys->connectItem( KStdAccel::Print , this, SLOT(slotPrint()) );
  keys->insertItem(i18n("Select All"),   "Select All",   CTRL+Key_A);
  keys->connectItem( "Select All", this, SLOT(slotSelectAll()) );
  keys->readSettings();

  kdDebug() << "setupMenuBar - read settings" << endl;

  recent = new QPopupMenu(menubar);
  connect( recent, SIGNAL(activated(int)), SLOT(slotOpenRecent(int)));
  bool recentExist=false;
  for ( QStringList::Iterator it = pprops->recentlist->begin();
  it != pprops->recentlist->end(); ++it )
    {
        recent->insertItem(*it);
	recentExist=true;
    }
  int id;

  file = new QPopupMenu(menubar);

  file->insertItem(SmallIcon("newwin"), i18n("New &Window"), ID_FILE_NEWWIN);
  file->connectItem(ID_FILE_NEWWIN, this, SLOT(slotNewWin()));

  file->insertSeparator();

  id = file->insertItem(SmallIcon("filenew"), i18n("&New..."), ID_FILE_NEWFILE);
  file->connectItem(ID_FILE_NEWFILE, this, SLOT(slotNew()));

  id = file->insertItem(SmallIcon("fileopen"), i18n("&Open..."), ID_FILE_OPEN);
  file->connectItem(ID_FILE_OPEN, this, SLOT(slotOpen()));

  file->insertItem(i18n("Open recent"), recent, ID_FILE_RECENT);

  file->insertSeparator();

  id = file->insertItem(SmallIcon("filesave"), i18n("&Save"), ID_FILE_SAVE);
  file->connectItem(ID_FILE_SAVE, this, SLOT(slotSave()));

  file->insertItem(i18n("S&ave as..."), ID_FILE_SAVEAS);
  file->connectItem(ID_FILE_SAVEAS, this, SLOT(slotSaveAs()));

  file->insertSeparator();

  id = file->insertItem(SmallIcon("fileprint"), i18n("&Print..."), ID_FILE_PRINT);
  file->connectItem(ID_FILE_PRINT, this, SLOT(slotPrint()));

  file->insertSeparator();

  id = file->insertItem(SmallIcon("fileclose"), i18n("&Quit"), ID_FILE_CLOSE);
  file->connectItem(ID_FILE_CLOSE, this, SLOT(slotClose()));

  menubar->insertItem(i18n("&File"), file);

  edit = new QPopupMenu(menubar);

  id = edit->insertItem(SmallIcon("editcut"), i18n("Cu&t"), ID_EDIT_CUT);
  edit->connectItem(ID_EDIT_CUT, this, SLOT(slotCut()));

  id = edit->insertItem(SmallIcon("editcopy"), i18n("&Copy"), ID_EDIT_COPY);
  edit->connectItem(ID_EDIT_COPY, this, SLOT(slotCopy()));

  id = edit->insertItem(SmallIcon("editpaste"), i18n("&Paste"), ID_EDIT_PASTE);
  edit->connectItem(ID_EDIT_PASTE, this, SLOT(slotPaste()));

  id = edit->insertItem(i18n("Paste as &new"), ID_EDIT_PASTE_AS_NEW);
  edit->connectItem(ID_EDIT_PASTE_AS_NEW, grid, SLOT(editPasteAsNew()));

  id = edit->insertItem(i18n("Clea&r"),  this, SLOT(slotClear()));

  edit->insertSeparator();

  id = edit->insertItem(i18n("Select &all"), ID_EDIT_SELECT_ALL);
  edit->connectItem(ID_EDIT_SELECT_ALL, this, SLOT(slotSelectAll()));

  menubar->insertItem(i18n("&Edit"), edit);

  zoom = new QPopupMenu(menubar);
  id = zoom->insertItem(i18n("1:1"), ID_VIEW_ZOOM_1TO1);
  id = zoom->insertItem(i18n("1:5"), ID_VIEW_ZOOM_1TO5);
  id = zoom->insertItem(i18n("1:10"), ID_VIEW_ZOOM_1TO10);
  connect( zoom, SIGNAL(activated(int)), SLOT(slotView(int)));

  view = new QPopupMenu(menubar);
  id = view->insertItem(SmallIcon("viewmag+"), i18n("Zoom &in"), ID_VIEW_ZOOM_IN);
  id = view->insertItem(SmallIcon("viewmag-"), i18n("Zoom &out"), ID_VIEW_ZOOM_OUT);
  id = view->insertItem(SmallIcon("viewmag"), i18n("Zoom factor"), zoom);
  connect( view, SIGNAL(activated(int)), SLOT(slotView(int)));
  menubar->insertItem(i18n("&View"), view);

  image = new QPopupMenu(menubar);

  id = image->insertItem(SmallIcon("transform"), i18n("&Resize"), ID_IMAGE_RESIZE);
  id = image->insertItem(SmallIcon("grayscale"), i18n("&GrayScale"), ID_IMAGE_GRAYSCALE);
  
  //id = image->insertItem(SmallIcon("kdepalette"), i18n("&Map to KDE palette"), ID_IMAGE_MAPTOKDE);
  
  connect( image, SIGNAL(activated(int)), SLOT(slotImage(int)));
  menubar->insertItem(i18n("&Icon"), image);

  tools = new QPopupMenu(menubar);
  tools->insertItem(SmallIcon("paintbrush"), i18n("Freehand"), ID_DRAW_FREEHAND);
  tools->insertItem(SmallIcon("rectangle"), i18n("Rectangle"), ID_DRAW_RECT);
  tools->insertItem(SmallIcon("filledrectangle"), i18n("Filled rectangle"), ID_DRAW_RECT_FILL);
  tools->insertItem(SmallIcon("circle"), i18n("Circle"), ID_DRAW_CIRCLE);
  tools->insertItem(SmallIcon("filledcircle"), i18n("Filled circle"), ID_DRAW_CIRCLE_FILL);
  tools->insertItem(SmallIcon("ellipse"), i18n("Ellipse"), ID_DRAW_ELLIPSE);
  tools->insertItem(SmallIcon("filledellipse"), i18n("Filled ellipse"), ID_DRAW_ELLIPSE_FILL);
  tools->insertItem(SmallIcon("spraycan"), i18n("Spray"), ID_DRAW_SPRAY);
  tools->insertItem(SmallIcon("flood"), i18n("Flood fill"), ID_DRAW_FILL);
  tools->insertItem(SmallIcon("line"), i18n("Line"), ID_DRAW_LINE);
  
  tools->insertItem(SmallIcon("eraser"), i18n("Eraser (Transparent)"), ID_DRAW_ERASE);

  connect( tools, SIGNAL(activated(int)), SLOT(slotTools(int)));
  menubar->insertItem(i18n("&Tools"), tools);

  options = new QPopupMenu(menubar);
  id = options->insertItem(i18n("&Configure"), ID_OPTIONS_CONFIGURE);
  options->insertSeparator();
  options->insertItem(i18n("Show &Grid"), ID_OPTIONS_TOGGLE_GRID);
  if(pprops->showgrid)
    options->setItemChecked(ID_OPTIONS_TOGGLE_GRID, true);
  options->insertItem(i18n("Show &toolbar"), ID_OPTIONS_TOGGLE_TOOL1);
  if(pprops->maintoolbarstat)
    options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL1, true);
  options->insertItem(i18n("Show &drawing tools"), ID_OPTIONS_TOGGLE_TOOL2);
  if(pprops->drawtoolbarstat)
    options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL2, true);
  options->insertItem(i18n("Show &statusbar"), ID_OPTIONS_TOGGLE_STATS);
  if(pprops->statusbarstat)
    options->setItemChecked(ID_OPTIONS_TOGGLE_STATS, true);
  options->insertSeparator();
  options->insertItem(SmallIcon("filesave"), i18n("&Save options"), ID_OPTIONS_SAVE);
  connect( options, SIGNAL(activated(int)), SLOT(slotConfigure(int)));
  menubar->insertItem(i18n("&Options"), options);

  menubar->insertSeparator();

  help = helpMenu();

  menubar->insertItem(i18n("&Help"), help);

  updateMenuAccel();
  menubar->show();
  //connect( menubar, SIGNAL(activated(int)), SLOT(slotActions(int)));
  file->setItemEnabled(ID_FILE_RECENT, recentExist);
  kdDebug() << "setupMenuBar - done" << endl;
  return true;
}

void KIconEdit::updateMenuAccel()
{
  KAccel *keys = props(this)->keys;  

  keys->changeMenuAccel(file, ID_FILE_NEWFILE, KStdAccel::New); 
  keys->changeMenuAccel(file, ID_FILE_OPEN, KStdAccel::Open); 
  keys->changeMenuAccel(file, ID_FILE_SAVE, KStdAccel::Save); 
  keys->changeMenuAccel(file, ID_FILE_PRINT, KStdAccel::Print); 
  keys->changeMenuAccel(file, ID_FILE_CLOSE, KStdAccel::Close); 
  keys->changeMenuAccel(file, ID_FILE_QUIT, KStdAccel::Quit); 

  keys->changeMenuAccel(edit, ID_EDIT_CUT, KStdAccel::Cut); 
  keys->changeMenuAccel(edit, ID_EDIT_COPY, KStdAccel::Copy); 
  keys->changeMenuAccel(edit, ID_EDIT_PASTE, KStdAccel::Paste); 
  keys->changeMenuAccel(edit, ID_EDIT_SELECT_ALL, "Select All"); 

}

bool KIconEdit::setupToolBar()
{
  Properties *pprops = props(this);
  kdDebug() << "setupToolBar" << endl;

  toolbar = toolBar();
  pprops->maintoolbar = toolbar;

  QWidget *btwhat = (QWidget*)what->whatsThisButton(toolbar);
  what = new QWhatsThis(btwhat);
  QToolTip::add(btwhat, i18n("What is ...?"));

  toolbar->insertButton(BarIcon("filenew"), ID_FILE_NEWFILE,
         SIGNAL(clicked()), this, SLOT(slotNew()), TRUE, i18n("New File"));
  toolbar->insertButton(BarIcon("fileopen"),ID_FILE_OPEN,
         SIGNAL(clicked()), this, SLOT(slotOpen()), TRUE, i18n("Open a file"));
  toolbar->insertButton(BarIcon("filesave"), ID_FILE_SAVE,
         SIGNAL(clicked()), this, SLOT(slotSave()), TRUE, i18n("Save the file"));
  toolbar->insertSeparator();
  toolbar->insertButton(BarIcon("fileprint"),ID_FILE_PRINT,
         SIGNAL(clicked()), this, SLOT(slotPrint()), TRUE, i18n("Print icon"));
  toolbar->insertSeparator();
  toolbar->insertButton(BarIcon("editcut"),ID_EDIT_CUT,
         SIGNAL(clicked()), this, SLOT(slotCut()), TRUE, i18n("Cut"));
  toolbar->insertButton(BarIcon("editcopy"),ID_EDIT_COPY,
         SIGNAL(clicked()), this, SLOT(slotCopy()), TRUE, i18n("Copy"));
  toolbar->insertButton(BarIcon("editpaste"),ID_EDIT_PASTE,
         SIGNAL(clicked()), this, SLOT(slotPaste()), TRUE, i18n("Paste"));

  select = new QPopupMenu(menubar);
  select->insertItem(SmallIcon("selectrect"), ID_SELECT_RECT);
  select->insertItem(SmallIcon("selectcircle"), ID_SELECT_CIRCLE);
  connect( select, SIGNAL(activated(int)), SLOT(slotTools(int)));

  toolbar->insertButton(BarIcon("areaselect"), ID_SELECT, select, true, i18n("Select area"));
  //drawtoolbar->setToggle(ID_DRAW_SELECT, true);

  toolbar->insertSeparator();

  toolbar->insertButton(BarIcon("transform"),ID_IMAGE_RESIZE, TRUE, 
			  i18n("Resize"));

  toolbar->insertButton(BarIcon("grayscale"),ID_IMAGE_GRAYSCALE, TRUE, 
			  i18n("GrayScale"));
/*
  toolbar->insertButton(BarIcon("kdepalette"),ID_IMAGE_MAPTOKDE, TRUE, 
			  i18n("To KDE palette"));
*/
  toolbar->insertSeparator();
  toolbar->insertButton(BarIcon("viewmag-"),ID_VIEW_ZOOM_OUT, TRUE, 
			  i18n("Zoom out"));
  toolbar->setDelayedPopup(ID_VIEW_ZOOM_OUT, zoom);
  toolbar->insertButton(BarIcon("viewmag+"),ID_VIEW_ZOOM_IN, TRUE, 
			  i18n("Zoom in"));
  toolbar->setDelayedPopup(ID_VIEW_ZOOM_IN, zoom);
  toolbar->insertSeparator();
  toolbar->insertButton(BarIcon("grid"),ID_OPTIONS_TOGGLE_GRID, TRUE, 
			  i18n("Toggle grid"));
  toolbar->setToggle(ID_OPTIONS_TOGGLE_GRID, true);
  if(pprops->showgrid)
    ((KToolBarButton*)toolbar->getButton(ID_OPTIONS_TOGGLE_GRID))->on(true);

  toolbar->insertSeparator();

//  toolbar->insertWidget(ID_HELP_WHATSTHIS, btwhat->sizeHint().width(), btwhat);

  toolbar->insertButton(BarIcon("newwin"),ID_FILE_NEWWIN,
         SIGNAL(clicked()), this, SLOT(slotNewWin()), TRUE,
                          i18n("New Window"));
  toolbar->alignItemRight( ID_FILE_NEWWIN, true);
    
  toolbar->setIconText((KToolBar::IconText) pprops->maintoolbartext);
  toolbar->setBarPos(pprops->maintoolbarpos);
  if(pprops->maintoolbarstat)
    toolbar->show();
  else
    toolbar->hide();
  connect( toolbar, SIGNAL(clicked(int)), SLOT(slotView(int)));
  connect( toolbar, SIGNAL(clicked(int)), SLOT(slotImage(int)));
  connect( toolbar, SIGNAL(clicked(int)), SLOT(slotConfigure(int)));

  kdDebug() << "setupToolBar - done" << endl;
  return true;
}

bool KIconEdit::setupDrawToolBar()
{
  kdDebug() << "setupDrawToolBar" << endl;
  Properties *pprops = props(this);
  drawtoolbar = toolBar("drawToolBar");
  pprops->drawtoolbar = drawtoolbar;
  drawtoolbar->setFullSize();

  drawtoolbar->insertButton(BarIcon("pointer"), ID_DRAW_FIND, TRUE, i18n("Find pixel"));
  drawtoolbar->setToggle(ID_DRAW_FIND, true);

  drawtoolbar->insertButton(BarIcon("paintbrush"), ID_DRAW_FREEHAND, TRUE, i18n("Draw freehand"));
  drawtoolbar->setToggle(ID_DRAW_FREEHAND, true);

  drawtoolbar->insertButton(BarIcon("line"), ID_DRAW_LINE, TRUE, i18n("Draw line"));
  drawtoolbar->setToggle(ID_DRAW_LINE, true);

  drawtoolbar->insertButton(BarIcon("rectangle"),ID_DRAW_RECT, TRUE, i18n("Draw rectangle"));
  drawtoolbar->setToggle(ID_DRAW_RECT, true);

  drawtoolbar->insertButton(BarIcon("filledrectangle"),ID_DRAW_RECT_FILL, TRUE, i18n("Draw filled rectangle"));
  drawtoolbar->setToggle(ID_DRAW_RECT_FILL, true);

  drawtoolbar->insertButton(BarIcon("circle"),ID_DRAW_CIRCLE, TRUE, i18n("Draw circle"));
  drawtoolbar->setToggle(ID_DRAW_CIRCLE, true);

  drawtoolbar->insertButton(BarIcon("filledcircle"),ID_DRAW_CIRCLE_FILL, TRUE, i18n("Draw filled circle"));
  drawtoolbar->setToggle(ID_DRAW_CIRCLE_FILL, true);

  drawtoolbar->insertButton(BarIcon("ellipse"),ID_DRAW_ELLIPSE, TRUE, i18n("Draw ellipse"));
  drawtoolbar->setToggle(ID_DRAW_ELLIPSE, true);

  drawtoolbar->insertButton(BarIcon("filledellipse"),ID_DRAW_ELLIPSE_FILL, TRUE, i18n("Draw filled ellipse"));
  drawtoolbar->setToggle(ID_DRAW_ELLIPSE_FILL, true);

  drawtoolbar->insertButton(BarIcon("spraycan"),ID_DRAW_SPRAY, TRUE, i18n("Spray"));
  drawtoolbar->setToggle(ID_DRAW_SPRAY, true);
  
  drawtoolbar->insertButton(BarIcon("flood"),ID_DRAW_FILL, TRUE, i18n("Flood fill"));
  drawtoolbar->setToggle(ID_DRAW_FILL, true);

  drawtoolbar->insertButton(BarIcon("eraser"),ID_DRAW_ERASE, TRUE, i18n("Erase"));
  drawtoolbar->setToggle(ID_DRAW_ERASE, true);

  drawtoolbar->setIconText((KToolBar::IconText) pprops->drawtoolbartext);
  drawtoolbar->setBarPos(pprops->drawtoolbarpos);

  if(pprops->drawtoolbarstat)
    drawtoolbar->show();
  else
    drawtoolbar->hide();
  connect( drawtoolbar, SIGNAL(clicked(int)), SLOT(slotTools(int)));

  kdDebug() << "setupDrawToolBar - done" << endl;
  return true;
}

bool KIconEdit::setupPaletteToolBar()
{
    QWidget *sysColorsWidget = new QWidget(this);
    QBoxLayout *ml = new QVBoxLayout(sysColorsWidget);
    QLabel *l = new QLabel(i18n("System Colors"), sysColorsWidget);
    syscolors = new KSysColors(sysColorsWidget);
    syscolors->setFixedSize(syscolors->width(), syscolors->height());
    ml->addWidget(l);
    ml->addWidget(syscolors);

    int ow = sysColorsWidget->sizeHint().width();
    int iw = syscolors->width();
    int ih = syscolors->height();

    palettetoolbar = toolBar("paletteToolbar");
    palettetoolbar->show();
    palettetoolbar->setBarPos(KToolBar::Right);

    sysColorsWidget->reparent(palettetoolbar, 0, QPoint(0, 0));

    preview = new Preview(palettetoolbar);
    preview->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    preview->setFixedSize(iw, 64); 
    palettetoolbar->insertWidget(ID_PREVIEW, iw, preview);
    palettetoolbar->insertSeparator();

    palettetoolbar->insertWidget(ID_SYSTEM_COLORS, iw, sysColorsWidget);

    QWidget *customColorsWidget = new QWidget(palettetoolbar);
    ml = new QVBoxLayout(customColorsWidget);
    l = new QLabel(i18n("Custom Colors"), customColorsWidget);
    customcolors = new KCustomColors(customColorsWidget);
    customColorsWidget->setFixedSize(iw, 80);
    ml->addWidget(l);
    ml->addWidget(customcolors);
    palettetoolbar->insertWidget(ID_CUSTOM_COLORS, iw, customColorsWidget);

    return true;
}

bool KIconEdit::setupStatusBar()
{
    Properties *pprops = props(this);
    statusbar = statusBar();
    pprops->statusbar = statusbar;

    statusbar->insertItem("    -1, -1    ", 0);
    statusbar->insertItem("   32 x 32   ", 1);
    statusbar->insertItem(" 1:1000 ", 2);
    statusbar->insertItem("Colors:       ", 3);
    statusbar->insertItem("", 4);

    if(pprops->statusbarstat)
        statusbar->show();
    else
        statusbar->hide();
    return true;
}

bool KIconEdit::setupWhatsThis()
{ // QWhatsThis *what is created in setupToolbar

  kdDebug() << "setupWhatsThis" << endl;

  // Create help for the custom widgets

  QString str = i18n("Icon draw grid\n\nThe icon grid is the area where you draw the icons.\n"
                 "You can zoom in and out using the magnifying glasses on the toolbar.\n"
                 "(Tip: Hold the magnify button down for a few seconds to zoom to a predefined scale");
  what->add(grid, str);

  str = i18n("Rulers\n\nThis is a visual representation of the current cursor position");
  what->add(gridview->hruler(), str);
  what->add(gridview->vruler(), str);

  str = i18n("Statusbar\n\nThe statusbar gives information on the status "
             "of the current icon. The fields are:\n\n"
             "\t- Cursor position\n"
             "\t- Size\n"
             "\t- Zoom factor\n"
             "\t- Number of colors\n"
             "\t- Application messages");

  what->add(statusbar, str);

  str = i18n("Preview\n\nThis is a 1:1 preview of the current icon");
  what->add(preview, str);

  str = i18n("System colors\n\nHere you can select colors from the KDE Icon Palette.");
  what->add(syscolors, str);

  str = i18n("Custom colors\n\nHere you can build a palette of custom colors.\n"
             "Double-click on a box to edit the color");
  what->add(customcolors, str);

  // Create help for the main toolbar

  str = i18n("New\n\nCreate a new icon, either from a template or by specifying the size");
  what->add(toolbar->getButton(ID_FILE_NEWFILE), str);

  str = i18n("Open\n\nOpen an existing icon");
  what->add(toolbar->getButton(ID_FILE_OPEN), str);

  str = i18n("Save\n\nSave the current icon");
  what->add(toolbar->getButton(ID_FILE_SAVE), str);

  str = i18n("Print\n\nOpens a print dialog to let you print the current icon."
             " (Doesn't really work as expected");
  what->add(toolbar->getButton(ID_FILE_PRINT), str);

  str = i18n("Cut\n\nCut the current selection out of the icon\n\n"
             "(Tip: You can make both rectangular and circular selections)");
  what->add(toolbar->getButton(ID_EDIT_CUT), str);

  str = i18n("Copy\n\nCopy the current selection out of the icon\n\n"
             "(Tip: You can make both rectangular and circular selections)");
  what->add(toolbar->getButton(ID_EDIT_COPY), str);

  str = i18n("Paste\n\nPaste the contents of the clipboard into the current icon.\n\n"
             "If the contents are larger than the current icon you can paste them"
             " in a new window.\n\n"
             "(Tip: Select \"Paste transparent pixels\" in the configuration dialog"
             " if you also want to paste transparency.)");
  what->add(toolbar->getButton(ID_EDIT_PASTE), str);

  str = i18n("Select\n\nSelect a section of the icon using the mouse.\n\n"
             "(Tip: You can make both rectangular and circular selections)");
  what->add(toolbar->getButton(ID_SELECT), str);

  str = i18n("Resize\n\nSmoothly resizes the icon while trying to preserve the contents");
  what->add(toolbar->getButton(ID_IMAGE_RESIZE), str);

  str = i18n("Gray scale\n\nGray scale the current icon.\n"
             "(Warning: The result is likely to contain colors not in the icon palette");
  what->add(toolbar->getButton(ID_IMAGE_GRAYSCALE), str);

  str = i18n("Zoom out\n\nZoom out by one.\n\n"
             "(Tip: Hold the button down for a few seconds to zoom to a predefined scale");
  what->add(toolbar->getButton(ID_VIEW_ZOOM_OUT), str);

  str = i18n("Zoom in\n\nZoom in by one.\n\n"
             "(Tip: Hold the button down for a few seconds to zoom to a predefined scale");
  what->add(toolbar->getButton(ID_VIEW_ZOOM_IN), str);

  str = i18n("Toggle grid\n\nToggles the grid in the icon edit grid on/off");
  what->add(toolbar->getButton(ID_OPTIONS_TOGGLE_GRID), str);

  str = i18n("New window\n\nOpens a new Icon Editor window.");
  what->add(toolbar->getButton(ID_FILE_NEWWIN), str);

  // Setup help for the tools toolbar

  str = i18n("Free hand\n\nDraw non-linear lines");
  what->add(drawtoolbar->getButton(ID_DRAW_FREEHAND), str);

  str = i18n("Flood fill\n\nFill adjoining pixels with the same color with the current color");
  what->add(drawtoolbar->getButton(ID_DRAW_FILL), str);

  str = i18n("Rectangle\n\nDraw a rectangle");
  what->add(drawtoolbar->getButton(ID_DRAW_RECT), str);

  str = i18n("Filled rectangle\n\nDraw a filled rectangle");
  what->add(drawtoolbar->getButton(ID_DRAW_RECT_FILL), str);

  str = i18n("Circle\n\nDraw a circle");
  what->add(drawtoolbar->getButton(ID_DRAW_CIRCLE), str);

  str = i18n("Filled circle\n\nDraw a filled circle");
  what->add(drawtoolbar->getButton(ID_DRAW_CIRCLE_FILL), str);

  str = i18n("Ellipse\n\nDraw an ellipse");
  what->add(drawtoolbar->getButton(ID_DRAW_ELLIPSE), str);

  str = i18n("Filled ellipse\n\nDraw a filled ellipse");
  what->add(drawtoolbar->getButton(ID_DRAW_ELLIPSE_FILL), str);

  str = i18n("Line\n\nDraw a straight line vertically, horizontally or at 45 deg. angles");
  what->add(drawtoolbar->getButton(ID_DRAW_LINE), str);

  str = i18n("Spray\n\nDraw scattered pixels in the current color");
  what->add(drawtoolbar->getButton(ID_DRAW_SPRAY), str);

  str = i18n("Find\n\nThe color of the pixel clicked on will be the current draw color");
  what->add(drawtoolbar->getButton(ID_DRAW_FIND), str);

  str = i18n("Erase\n\nErase pixels. Set the pixels to be transparent\n\n"
             "(Tip: If you want to draw transparency with a different tool,"
             " first click on \"Erase\" then on the tool you want to use)");
  what->add(drawtoolbar->getButton(ID_DRAW_ERASE), str);

  kdDebug() << "setupWhatsThis - done" << endl;
  return true;
}

void KIconEdit::addRecent(const QString & path)
{
    QString pathStr = path;

    kdDebug() << "KIconEdit::addRecent() - checking " << pathStr << endl;
    Properties *pprops = props(this);
    if(pathStr.isEmpty() || pprops->recentlist->contains(pathStr))
        return;

    kdDebug() << "KIconEdit::addRecent() - adding to pprops->recentlist " << pathStr << endl;
    if( pprops->recentlist->count() < 5)
    {
        pprops->recentlist->prepend(pathStr);
    }    
    else
    {
        pprops->recentlist->remove(pprops->recentlist->at(4));
        pprops->recentlist->prepend(pathStr);
    }
    kdDebug() << "KIconEdit::addRecent() - adding to pprops->recentlist- done " << path << endl;
    recent->clear();

    kdDebug() << "KIconEdit::addRecent() - adding to recent " << path << endl;
    for ( int i = 0 ;i < (int)pprops->recentlist->count(); i++)
        recent->insertItem(*pprops->recentlist->at(i));
        
    kdDebug() << "KIconEdit::addRecent() - adding to recent - done" << path << endl;

    file->setItemEnabled(ID_FILE_RECENT, true);
    kdDebug() << "addRecent - done" << endl;
}


void KIconEdit::toggleTool(int id)
{
  for(int i = ID_DRAW_FREEHAND; i <= ID_DRAW_ERASE; i++)
  {
        if(i != id)
        {
        if(tools->isItemChecked(i))
            tools->setItemChecked(i, false);
        if(drawtoolbar->isButtonOn(i));
            drawtoolbar->setButton(i, false);
        //((KToolBarButton*)drawtoolbar->getButton(i))->on(false);
    }
    else
    {
        tools->setItemChecked(i, true);
        if(!drawtoolbar->isButtonOn(i));
        //((KToolBarButton*)drawtoolbar->getButton(i))->on(true);
            drawtoolbar->setButton(i, true);
    }
  }
}
#include "kiconedit.moc"
