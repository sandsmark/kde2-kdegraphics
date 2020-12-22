   /*

    $Id: kfax.cpp 95763 2001-05-08 02:42:27Z jsinger $

    Copyright (C) 1997 Bernd Johannes Wuebben
                       wuebben@math.cornell.edu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */

#ifdef KDE_USE_FINAL
/* NewImage() in viewfax.cpp needs to fiddle with the Display structure */
#define XLIB_ILLEGAL_ACCESS
#endif

#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include <qfile.h>
#include <qstrlist.h>
#include <qdragobject.h>

#include <klocale.h>
#include <kaboutdata.h>
#include <kstddirs.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kstdaccel.h>
#include <kconfig.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kcmdlineargs.h>
#include <kio/netaccess.h>
#include <knotifyclient.h>
#include <ktempfile.h>

#include "kfax.h"
#include "kfax.moc"
#include "version.h"
#include "viewfax.h"

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifndef _PATH_TMP
#define _PATH_TMP "/tmp/"
#endif

TopLevel *toplevel;
QFrame *faxqtwin;

extern  int GetImage(struct pagenode *pn);

void 	TurnFollowing(int How, struct pagenode *pn);
void 	handle_X_event(XEvent event);
int 	notetiff(const char *name);
void 	ShowLoop(void);
void 	SetupDisplay();
void    catchSignals();
void 	mysighandler(int sig);
void    setFaxDefaults();

void 	parse(char* buf, char** args);
void 	setfaxtitle(const QString& name);
int     copyfile(const char* to,char* from);

extern  "C"{
int    fax2psmain(const char* faxtiff_file,FILE* psoutput, float width,float height,int scale);
int    fax2tiffmain(const char* inputfile, const char* outputfile,int bitorder,int stretchit,int type);
}

extern void g31expand(struct pagenode *pn, drawfunc df);
extern void g32expand(struct pagenode *pn, drawfunc df);
extern void g4expand(struct pagenode *pn, drawfunc df);

#define PATIENCE 100000


#undef 	min
#undef 	max
#define min(a,b)	((a)<(b)?(a):(b))
#define max(a,b)	((a)>(b)?(a):(b))

int 	ExpectConfNotify = 1;

GC 	PaintGC;
Cursor 	WorkCursor;
Cursor 	ReadyCursor;
Cursor 	MoveCursor;
Cursor 	LRCursor;
Cursor 	UDCursor;

extern  int abell;			/* audio bell */
extern 	int vbell;			/* visual bell */

extern Time Lasttime;
extern bool have_cmd_opt;
extern  struct pagenode *viewpage;

extern 	XImage *FlipImage(XImage *xi);
extern 	XImage *MirrorImage(XImage *xi);
extern 	XImage *RotImage(XImage *Image);
extern 	XImage *ZoomImage(XImage *Big);
extern 	void FreeImage(XImage *Image);

#define MAXZOOM	10

extern 	XImage *Image, *Images[MAXZOOM];

int 	xpos= 0, ox= 0;	/* x, old x, offset x, new xpos*/
int 	ypos= 0, oy = 0;	/* y, old y, offset y, new y */
int 	offx = 0;
int 	offy = 0;
int 	nx = 0;
int 	ny = 0;

int oz, Resize = 0, Refresh = 0;	/* old zoom, window size changed,
					   needs updating */
int PaneWidth, PaneHeight;		/* current size of our window */
int AbsX, AbsY;				/* absolute position of centre of window */

int Reparented = 0;

Display* qtdisplay;

int 	startingup;
Window  qtwin;    // the qt window
Window 	Win;
int 	counter = 0;
int 	qwindow_height;
int 	qwindow_width;
bool 	have_no_fax = TRUE;
QString fname;
bool	display_is_setup = FALSE;
struct 	optionsinfo fop;   // contains the fax options

extern 	struct pagenode *firstpage, *lastpage, *thispage, *helppage;
extern  struct pagenode* auxpage;

extern 	struct pagenode defaultpage;
extern  size_t Memused;

extern int zfactor;
int faxpagecounter = 0;
bool buttondown;

bool MyApp::x11EventFilter( XEvent * ev){

  /*  printf("Event  %d for window %d\n", ev->type, ev->xany.window);*/

  if (KApplication::x11EventFilter(ev))
    return TRUE;

  if (ev->type  ==  ButtonRelease){
    /* this is so that the cursor goes back to normal on leaving the fax window
       and that the fax won't be moved when I reenter after I release the mouse*/

    if (buttondown == true){
      buttondown = false;
      XDefineCursor(qtdisplay, Win, ReadyCursor);
      XFlush(qtdisplay);
    }
  }
  if ( ev->xany.window == qtwin ||
       ev->xany.window == Win){

    if(startingup || have_no_fax)
      return FALSE;

    toplevel->handle_X_event(*ev);
    ev->xany.window = qtwin;

  }

  return FALSE;

}


TopLevel::TopLevel (QWidget *, const char *name)
    : KMainWindow (0, name)
{
  setMinimumSize (100, 100);

  file_dialog = NULL;
  printdialog = NULL;
  buttondown = false;
  statusbar_timer = new QTimer(this);
  connect(statusbar_timer, SIGNAL(timeout()),this,SLOT(timer_slot()));

  connect(kapp,SIGNAL(kdisplayPaletteChanged()),this,SLOT(set_colors()));

  setupMenuBar();
  setupToolBar();
  setupStatusBar();

  readSettings();

  faxqtwin = new QFrame(this);

  qtwin = faxqtwin->winId();
  faxqtwin->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  // Create a Vertical scroll bar

  vsb = new QScrollBar( QScrollBar::Vertical,faxqtwin,"scrollBar" );
  vsb->hide();
  connect( vsb, SIGNAL(valueChanged(int)), SLOT(scrollVert(int)) );

  // Create a Horizontal scroll bar

  hsb = new QScrollBar( QScrollBar::Horizontal,faxqtwin,"scrollBar" );
  connect( hsb, SIGNAL(valueChanged(int)), SLOT(scrollHorz(int)) );
  hsb->hide();


  setCentralWidget(faxqtwin);
  set_colors();

  setCaption(i18n("KFax"));

  setAcceptDrops(true);

  // Calling resize as is done here will reduce flicker considerably
  // ktoplevelwidget updateRect will be called only twice this way instead of 4 times.
  // If I leave the first resize out the toolbar will not be displayed and
  // ktoplevelwidget updateRect will be called four times. All this needs to
  // be looked into. ktoolbar <-> ktoplevelwidget

  resize(editor_width,editor_height);
  show();
  resize(editor_width,editor_height);
}


TopLevel::~TopLevel (){

  delete file;
  delete help;
  delete options;
}

void TopLevel::pageActivated(const QString&){


}

void TopLevel::setFaxTitle(const QString& name){

  QString str(i18n("KFax: "));
  str.append(name);
  setCaption(str);
}

void TopLevel::setupEditWidget(){


}


void TopLevel::resizeEvent(QResizeEvent *e){

  KMainWindow::resizeEvent(e);

  if(!faxqtwin || !display_is_setup)
    return;

  resizeView();
}

void TopLevel::resizeView(){


  if(!faxqtwin || !display_is_setup)
    return;

//printf("In resizeView() entered\n");

  qwindow_width = faxqtwin->width();
  qwindow_height = faxqtwin->height();

  if( hsb->isVisible())
    qwindow_height -= 16;

  if( vsb->isVisible())
    qwindow_width  -= 16;

  if(Image){
    PaneWidth = Image->width;
    PaneHeight = Image->height;
  }

  //  printf("faxw %d qtw %d\n", PaneWidth , faxqtwin->width());

  if( (PaneHeight  > qwindow_height ) &&
      (PaneWidth  > qwindow_width)){

    vsb->setGeometry(faxqtwin->width() - 16,0,16,faxqtwin->height()-16);
    hsb->setGeometry(0,faxqtwin->height() - 16 ,faxqtwin->width()-16,16);

    qwindow_width = faxqtwin->width() -16;
    qwindow_height = faxqtwin->height()-16;


    vsb->raise();
    vsb->show();
    hsb->show();
  }
  else{


    if( PaneHeight  > qwindow_height){
      vsb->setGeometry(faxqtwin->width() - 16,0,16,faxqtwin->height());


      qwindow_width = faxqtwin->width() -16 ;
      qwindow_height = faxqtwin->height();


      vsb->show();
      hsb->hide();
      hsb->raise();
    }
     else
       vsb->hide();

    if( PaneWidth  > qwindow_width ){
      hsb->setGeometry(0,faxqtwin->height() - 16 ,faxqtwin->width(),16);
      hsb->show();
      hsb->raise();
      vsb->hide();
      qwindow_width = faxqtwin->width() ;
      qwindow_height = faxqtwin->height() -16;

    }
    else
      hsb->hide();

  }

  if(Image){
    hsb->setRange(0,max(0,Image->width - qwindow_width));
    hsb->setSteps(5,qwindow_width/2);
    //    printf("hsb range: %d\n",max(0,Image->width - qwindow_width));
    vsb->setRange(0,max(0,Image->height - qwindow_height));
    vsb->setSteps(5,qwindow_height/2);
    //    printf("vsb range: %d\n",max(0,Image->height - qwindow_height));
    //    printf("vsb min %d vdb max %d\n",vsb->minValue(),vsb->maxValue());
  }


  Resize = 1;
  uiUpdate();

}



void TopLevel::setupMenuBar(){

  file = 	new QPopupMenu ();
  help = 	new QPopupMenu ();
  view =        new QPopupMenu ();
  options = 	new QPopupMenu ();

  help = helpMenu();

  help->insertItem (i18n("Help page"), 		this, 	SLOT(helpshort()));

  file->insertItem (i18n("&Open Fax..."),		this, 	SLOT(file_open()),KStdAccel::open());
  file->insertItem (i18n("&Add Fax..."),		this, 	SLOT(addFax()));
  file->insertItem (i18n("&Save Fax..."),		this, 	SLOT(file_save()));
  file->insertItem (i18n("&Close Fax"),		this,	SLOT(closeFax()),KStdAccel::close());
  file->insertSeparator (-1);
  file->insertItem (i18n("&Print..."),	this,	SLOT(print()),KStdAccel::print() );

  file->insertSeparator (-1);
  file->insertItem (i18n("E&xit"), 		this,	SLOT(quiteditor()),KStdAccel::quit());

  view->insertItem(i18n("&Next Page"),this,SLOT(nextPage()));
  view->insertItem(i18n("&Prev Page"),this,SLOT(prevPage()));
  view->insertItem(i18n("&First Page"),this,SLOT(goToStart()));
  view->insertItem(i18n("&Last Page"),this,SLOT(goToEnd()));
  view->insertSeparator(-1);
  view->insertItem(i18n("&Rotate Page"),this,SLOT(rotatePage()));
  view->insertItem(i18n("&Mirror Page"),this,SLOT(mirrorPage()));
  view->insertItem(i18n("&Flip Page"),this,SLOT(flipPage()));
  view->insertSeparator(-1);
  view->insertItem(i18n("Zoom &in"),this,SLOT(zoomin()));
  view->insertItem(i18n("Zoom &out"),this,SLOT(zoomout()));



  options->setCheckable(TRUE);
  options->insertItem(i18n("&Fax Options..."),this,SLOT(faxoptions()));
  options->insertSeparator(-1);
  toolID   = options->insertItem(i18n("&Tool Bar"),this,SLOT(toggleToolBar()));
  statusID = options->insertItem(i18n("&Status Bar"),this,SLOT(toggleStatusBar()));	


  menubar = new KMenuBar (this, "menubar");
  menubar->insertItem (i18n("&File"), file);
  menubar->insertItem (i18n("&View"), view);
  menubar->insertItem (i18n("&Options"), options);
  menubar->insertSeparator(-1);
  menubar->insertItem (i18n("&Help"), help);
}


void TopLevel::setupToolBar()
{
    toolBar()->insertButton(BarIcon("back"), ID_PREV, TRUE, i18n("Go back"));
    toolBar()->insertButton(BarIcon("forward"), ID_NEXT, TRUE, i18n("Go forward"));

    toolBar()->insertSeparator();

    toolBar()->insertButton(BarIcon("start"), ID_START, TRUE, i18n("Go to start"));
    toolBar()->insertButton(BarIcon("finish"), ID_END, TRUE, i18n("Go to end"));

    toolBar()->insertSeparator();

    toolBar()->insertButton(BarIcon("viewmag+"), ID_ZOOM_IN, TRUE,i18n( "Zoom in"));
    toolBar()->insertButton(BarIcon("viewmag-"), ID_ZOOM_OUT, TRUE, i18n("Zoom out"));

    toolBar()->insertSeparator();

    toolBar()->insertButton(BarIcon("fileprint"), ID_PRINT, TRUE, i18n("Print document"));

    /*
	toolBar()->insertButton(BarIcon("tick"), ID_MARK, TRUE, "Mark this page");
    */
    toolBar()->insertSeparator();

    toolBar()->insertButton(BarIcon("page"), ID_PAGE, TRUE, i18n("Go to page ..."));

    toolBar()->setBarPos( KToolBar::Top );
	
    connect(toolBar(), SIGNAL( clicked( int ) ), this,
          SLOT( toolbarClicked( int ) ) );

    connect(toolBar(), SIGNAL( moved( BarPosition ) ), this,
          SLOT( toolbarMoved(BarPosition ) ) );
}

void TopLevel::toolbarMoved(BarPosition  ){

  resizeView();

}

void TopLevel::toolbarClicked( int item ){

	switch ( item ) {
  		case ID_PREV:
		        prevPage();
  			break;
  		case ID_NEXT:
		        nextPage();
  			break;
  		case ID_PAGE:
		        helpselected();
  			break;
  		case ID_ZOOM_IN:
		        zoomin();
  			break;
  		case ID_ZOOM_OUT:
		        zoomout();
  			break;
  		case ID_PRINT:
  			print();
  			break;
  		case ID_START:
		        goToStart();
  			break;
  		case ID_END:
		        goToEnd();
  			break;
  		case ID_READ:
  			break;
  	}
}

void TopLevel::setupStatusBar(){
    //    statusBar()->insertFixedItem("Mem: 000000000", ID_LINE_COLUMN);
    statusBar()->insertFixedItem(i18n("w: 00000 h: 00000"), ID_INS_OVR);
    statusBar()->insertFixedItem(i18n("Res: XXXXX"), ID_GENERAL);
    statusBar()->insertFixedItem(i18n("Type: XXXXXXX"), ID_TYPE);
    statusBar()->insertFixedItem(i18n("P:XXXofXXX"), ID_PAGE_NO);
    statusBar()->insertItem("",ID_FNAME, 1);

    //    statusBar()->changeItem("", ID_LINE_COLUMN);
    statusBar()->changeItem("", ID_INS_OVR);
    statusBar()->changeItem("", ID_GENERAL);
    statusBar()->changeItem("", ID_TYPE);
    statusBar()->changeItem("", ID_PAGE_NO);
    statusBar()->changeItem("",ID_FNAME);

    //statusBar()->setInsertOrder(KStatusBar::RightToLeft);

    //    statusBar()->setAlignment(ID_INS_OVR,AlignCenter);

    //    statusBar()->setInsertOrder(KStatusBar::LeftToRight);
    //    statusBar()->setBorderWidth(1);
}

void TopLevel::quiteditor(){

  writeSettings();
  kapp->quit();

}

void TopLevel::file_save()
{
  KURL url = KFileDialog::getOpenURL(current_directory,"*");

  if (url.isEmpty())
    return;

  if( url.isLocalFile() )
  {
    current_directory =  url.directory();
  }

  saveNetFile(url);  
}


void TopLevel::file_open()
{
  KURL url = KFileDialog::getOpenURL(current_directory,"*");

  if (url.isEmpty())
    return;

  if( url.isLocalFile() )
  {
    current_directory = url.directory();
  }
  closeFax();

  openNetFile(url);  
}

void TopLevel::addFax()
{
  KURL url = KFileDialog::getOpenURL(current_directory,"*");

  if (url.isEmpty())
    return;

  if( url.isLocalFile() )
  {
    current_directory = url.directory();
  }
  openNetFile(url);  
}


void TopLevel::openadd(QString filename){

  bool this_is_the_first_fax;

  if(firstpage == lastpage)
    this_is_the_first_fax = TRUE;
  else
    this_is_the_first_fax = FALSE;


  auxpage = lastpage;

  (void) notetiff(QFile::encodeName(filename));	

  if(!this_is_the_first_fax){
    if(auxpage->next)
      auxpage = auxpage->next;
  }

  // auxpage should now point to the first pagenode which was created for
  // the newly added fax file.


  have_no_fax = false;
  thispage = auxpage;
  newPage();
  resizeView();
  putImage();
}

void TopLevel::setGeneralStatusField(QString text){

    statusbar_timer->stop();
    statusBar()->changeItem(text,ID_FNAME);
    statusbar_timer->start(10000,TRUE); // single shot
}


void TopLevel::helpselected()
{
  kapp->invokeHelp( );
}

void TopLevel::helpshort()
{
  helptiff();
}

void TopLevel::toggleStatusBar()
{
  if(hide_statusbar) {
    hide_statusbar=FALSE;
    statusBar()->show();
    options->changeItem(i18n("Hide &Status Bar"), statusID);

  }
  else {
    hide_statusbar=TRUE;
    statusBar()->hide();
    options->changeItem(i18n("Show &Status Bar"), statusID);
  }
  resizeView();
}


void TopLevel::dummy(){
  KMessageBox::sorry(this, i18n("Not yet implemented"));
}

void TopLevel::toggleToolBar(){

  if(hide_toolbar) {
    hide_toolbar=FALSE;
    toolBar()->show();
    options->changeItem(i18n("Hide &Tool Bar"), toolID);
  }
  else {
    hide_toolbar=TRUE;
    toolBar()->hide();
    options->changeItem(i18n("Show &Tool Bar"), toolID);
  }

  resizeView();
}	


void TopLevel::closeEvent( QCloseEvent *e )
{
  (void) e;

  writeSettings();
  kapp->quit();
};


void TopLevel::setStatusBarMemField(int /*mem*/){

    //    QString memstr = i18n("Mem: %1").arg(mem);
    //    statusBar()->changeItem(memstr, ID_LINE_COLUMN);

}


void TopLevel::print(){
  if(!thispage){
    KMessageBox::sorry(this, i18n("There is no document active."));
    return;
  }

  if(!printdialog){
    printdialog = new PrintDialog(NULL,"print",true);
    connect(printdialog,SIGNAL(print()),this,SLOT(printIt()));

  }

  struct printinfo newpi;

  newpi.file = pi.file;
  newpi.cmd = pi.cmd;
  newpi.lpr = pi.lpr;
  newpi.scale = pi.scale;
  newpi.pagesize = pi.pagesize;
  newpi.margins = pi.margins;
  newpi.xmargin = pi.xmargin;
  newpi.ymargin = pi.ymargin;

  printdialog->setWidgets(&newpi);

  QPoint point = this->mapToGlobal (QPoint (0,0));

  QRect pos = this->geometry();
  printdialog->setGeometry(point.x() + pos.width()/2  - printdialog->width()/2,
			   point.y() + pos.height()/2 - printdialog->height()/2,
			   printdialog->width(),
			   printdialog->height()
			   );

  printdialog->show();

}

void TopLevel::printIt(){

  if(!firstpage)
    return;

  if(!printdialog)
    return;

  kapp->processEvents();

  struct printinfo *newpi;

  newpi = printdialog->getInfo();

  pi.file = newpi->file;
  pi.cmd = newpi->cmd;
  pi.lpr = newpi->lpr;
  pi.scale = newpi->scale;
  pi.pagesize = newpi->pagesize;
  pi.margins = newpi->margins;
  pi.xmargin = newpi->xmargin;
  pi.ymargin = newpi->ymargin;


  float width = 8.5;
  float height = 11;

  if(pi.pagesize ==  US_LETTER){
    width = 8.5;
    height = 11;
  }

  if(pi.pagesize == US_LEGAL){
    width = 8.5;
    height = 14;
  }

  if(pi.pagesize == US_LEDGER){
    width = 11;
    height = 17;
  }

  if(pi.pagesize == US_EXECUTIVE){
    width = 7.25;
    height = 10.5;
  }

  if(pi.pagesize == DIN_A3){
    width = 12.69;
    height = 16.53;
  }

  if(pi.pagesize == DIN_A4){
    width = 8.267;
    height = 11.692;
  }

  if(pi.pagesize == DIN_A5){
    width = 5.944;
    height = 8.2675;
  }

  if(pi.pagesize == DIN_A6){
    width = 4.2125;
    height = 5.8258;
  }

  if(pi.pagesize == DIN_B4){
    width = 10.04;
    height = 14.33;
  }

  if(pi.pagesize == JAP_LETTER){
    width = 7.165;
    height = 10.1175;
  }

  if(pi.pagesize == JAP_LEGAL){
    width = 10.1175;
    height = 14.33;
  }


  if(pi.margins == 1){
    width  = width - pi.xmargin/2.54*2.0; /* 2.0 since we want this margin both at the
					     top as well as on the bottom */
    height = height - pi.ymargin/2.54*2.0;
  }

  if(width <= 0.0 || height <=0.0 ){

    QString str = i18n("Invalid page dimensions:\nWidth %1 Height %2\n")
		.arg(width, 2).arg(height, 2);
    KMessageBox::sorry(this, str);

    return;
  }

  FILE* psfile;

  if(pi.lpr){

    psfile = NULL;
    psfile = popen(QFile::encodeName(pi.cmd),"w");

    if(psfile == NULL){
      QString str = i18n("Can't print to \"%1\"\n").arg(pi.cmd);
      KMessageBox::sorry(this, str);
      return;
    }

  }
  else{

    psfile = fopen(QFile::encodeName(pi.file),"w");
    if(psfile == NULL){
      QString str = i18n("Couldn't create %1\n").arg(pi.file);
      KMessageBox::error(this, str);
      return;
    }
  }

  QApplication::setOverrideCursor( waitCursor );
  XDefineCursor(qtdisplay, Win, WorkCursor);
  kapp->processEvents();

  struct pagenode *pn;
  QStrList filelist;

  for(pn = firstpage; pn; pn = pn->next){

    if(filelist.find(pn->pathname) != -1) // we have printed this fax file already
      continue;

    filelist.append(pn->pathname);

    if(pn->type == FAX_TIFF){

      fax2psmain(pn->pathname,psfile,width,height,pi.scale);

    }

    if(pn->type == FAX_RAW){

      int faxtype = 31;

      if(defaultpage.expander ==  g32expand)
	faxtype = 32;

      if(defaultpage.expander ==  g4expand)
	faxtype = 4;

      if(defaultpage.expander ==  g31expand)
	faxtype = 31;

      KTempFile tmpFile;
      tmpFile.setAutoDelete(true);

      fax2tiffmain(pn->pathname,QFile::encodeName(tmpFile.name()),pn->lsbfirst,pn->vres?0:1,faxtype);
      fax2psmain(QFile::encodeName(tmpFile.name()),psfile,width,height,pi.scale);
    }

  }

  if(pi.lpr)
    pclose(psfile);
  else
    fclose(psfile);

  QApplication::restoreOverrideCursor();
  XDefineCursor(qtdisplay, Win, ReadyCursor);

}


void TopLevel::setSensitivity (){

}


void TopLevel::saving_slot(){

  setGeneralStatusField(i18n("Saving ..."));

}


void TopLevel::loading_slot(){

  setGeneralStatusField(i18n("Loading ..."));

}


void TopLevel::saveNetFile( const KURL& u)
{
    if ( u.isMalformed() )
    {
	KMessageBox::sorry(this, i18n("Malformed URL"));
	return;
    }

    // Just a usual file ?
    if ( u.isLocalFile() )
      {
        saving_slot();
	int res = copyfile(QFile::encodeName(u.path()),thispage->pathname);
	if (res==0) {
	  KMessageBox::error(this, i18n("Failure in 'copy file()'\n"
				"Couldn't save file!"));
          setGeneralStatusField(i18n("Error saving"));
	  return;
	}

	setGeneralStatusField(i18n("Saved"));
	return;
      }

    KTempFile tmpFile;
    tmpFile.setAutoDelete(true);

    int res = copyfile(QFile::encodeName(tmpFile.name()),thispage->pathname );
    if (res==0) {
      KMessageBox::error(this, i18n("Failure in 'copy file()'\n"
			    "Couldn't save file!"));
      return;
    }

    KIO::NetAccess::upload( tmpFile.name(), u );
}

void TopLevel::openNetFile( const KURL &u)
{
  if ( u.isMalformed() )
    {
	KMessageBox::error(this, i18n("Malformed URL"));
	return;
    }

  // Just a usual file ?
  if ( u.isLocalFile() )
  {
      QString string = i18n("Loading '%1'").arg(u.path());
      setGeneralStatusField(string);
      openadd( u.path());
      setGeneralStatusField(i18n("Done"));
      return;
  }

  setGeneralStatusField(i18n("Downloading..."));
  QString tmpFile = QString::null; // let NetAccess decide about it
  if ( KIO::NetAccess::download( u, tmpFile ) )
  {
    setGeneralStatusField( i18n("Finished '%1'").arg(u.prettyURL()) );
    openadd( tmpFile );
    setCaption( u.prettyURL() );
	
    // Clean up
    KIO::NetAccess::removeTempFile( tmpFile );
  }
}


void TopLevel::dropEvent( QDropEvent * event)
{

  QStrList list;
  QUriDrag::decode(event, list);

  const char *s = list.first();

   if(s != 0L){

     // Load the first file in this window

     if ( s == list.getFirst() )
       {
	 QString n = s;
	 openNetFile( n );

       }
   }
   // lets ignore others for the moment.... ;-)
}

void TopLevel::timer_slot(){

  if(thispage){
    if(thispage->name)
      statusBar()->changeItem(thispage->name,ID_FNAME);
  }
  else
      statusBar()->changeItem("",ID_FNAME);

}


void TopLevel::set_colors(){

  /*
  QPalette mypalette = (eframe->palette()).copy();

  QColorGroup cgrp = mypalette.normal();
  QColorGroup ncgrp(forecolor,cgrp.background(),
		    cgrp.light(),cgrp.dark(),cgrp.mid(),forecolor,backcolor);

  mypalette.setNormal(ncgrp);
  mypalette.setDisabled(ncgrp);
  mypalette.setActive(ncgrp);

  eframe->setPalette(mypalette);
  eframe->setBackgroundColor(backcolor);
  */
}


void TopLevel::readSettings()
{
	config = KGlobal::config();

	///////////////////////////////////////////////////
	config->setGroup("General Options");

	current_directory = config->readEntry("DefaultPath", QDir::currentDirPath());
	editor_width = config->readNumEntry("Width", 550);
	editor_height = config->readNumEntry("Height", 400);

        // Inverse logic. Default is "show statusbar".
        hide_statusbar = config->readBoolEntry( "StatusBar", true );
        toggleStatusBar();

        // Inverse logic. Default is "show toolbar".
        hide_toolbar = config->readBoolEntry("ToolBar", true);
        toggleToolBar();

	QString str = config->readEntry( "Toolbar position" );
	if ( !str.isNull() ) {
		if( str == "Left" ) {
			toolBar()->setBarPos( KToolBar::Left );
		} else if( str == "Right" ) {
			toolBar()->setBarPos( KToolBar::Right );
		} else if( str == "Bottom" ) {
			toolBar()->setBarPos( KToolBar::Bottom );
		} else
			toolBar()->setBarPos( KToolBar::Top );
	}			

	///////////////////////////////////////////////////
	config->setGroup("Fax Options");

	fop.width = config->readNumEntry("width", 1728);
	fop.resauto = config->readNumEntry("resauto", 1);
	fop.geomauto = config->readNumEntry("geomauto", 1);
	fop.height = config->readNumEntry("height", 2339);
	fop.fine = config->readNumEntry("resolution", 1);
	fop.flip = config->readNumEntry("flip", 0);
	fop.invert = config->readNumEntry("invert", 0);
	fop.lsbfirst = config->readNumEntry("lsb", 0);
	fop.raw = config->readNumEntry("raw", 3);

	setFaxDefaults();
	///////////////////////////////////////////////////

	config->setGroup("Printing");

	pi.cmd = config->readEntry("PrintCommand", "lpr");
	pi.file = config->readEntry("PrintFile");
	pi.lpr = config->readNumEntry("print", 1);
	pi.scale = config->readNumEntry("scale", 0);
	pi.margins = config->readNumEntry("margins", 0);
	pi.xmargin = config->readNumEntry("xmargin", 0.0);
	pi.ymargin = config->readNumEntry("ymargin", 0.0);
	pi.pagesize = config->readEntry("PageSize",US_LETTER);
}

void TopLevel::writeSettings(){
		

  	config = kapp->config();
	
	///////////////////////////////////////////////////

	
	config->setGroup("General Options");

        config->writeEntry("DefaultPath", current_directory);
	config->writeEntry("Width", this->width());
	config->writeEntry("Height", this->height());
	config->writeEntry("StatusBar", !hide_statusbar);
	config->writeEntry("ToolBar", !hide_toolbar);

	QString string;
	if ( toolBar()->barPos() == KToolBar::Left )
		string = "Left";
	else if ( toolBar()->barPos() == KToolBar::Right )
		string = "Right";
	else if ( toolBar()->barPos() == KToolBar::Bottom )
		string = "Bottom";
	else
		string = "Top";

	config->writeEntry( "Toolbar position", string );

	////////////////////////////////////////////////////

	config->setGroup("Fax Options");

	config->writeEntry("resauto",fop.resauto);
	config->writeEntry("geomauto",fop.geomauto);
	config->writeEntry("width",fop.width);
	config->writeEntry("height",fop.height);
	config->writeEntry("resolution",fop.fine);
	config->writeEntry("flip",fop.flip);
	config->writeEntry("invert",fop.invert);
	config->writeEntry("lsb",fop.lsbfirst);
	config->writeEntry("raw",fop.raw);

       ////////////////////////////////////////////

	config->setGroup("Printing");

	config->writeEntry("PrintCommand",pi.cmd);
	config->writeEntry("PrintFile",pi.file);
	config->writeEntry("print",pi.lpr);
	config->writeEntry("scale",pi.scale);
	config->writeEntry("PageSize",pi.pagesize);
	config->writeEntry("margins",pi.margins);
	config->writeEntry("xmargin", pi.xmargin);
	config->writeEntry("ymargin", pi.ymargin);
	config->sync();	
}


void SetupDisplay(){

  if(display_is_setup){
    return;
  }

  display_is_setup = TRUE;

  xpos = ypos = ox = oy = 0;
  ExpectConfNotify = 1;

  /*  XSizeHints size_hints;*/

  zfactor = 4; // a power of two
  // the original image size is zfactor 0 the next small size (half as large)
  // is zfactor 1 etc.

  /*
  int faxh = Pimage(thispage)->height;
  int faxw = Pimage(thispage)->width;

  // TODO Let the user choose this in a settings dialog


  int i;
  for (size_hints.width = faxw, i = 1; i < zfactor; i *= 2)
    size_hints.width = (size_hints.width + 1) /2;

  for (size_hints.height = faxh, i = 1; i < zfactor; i *= 2)
    size_hints.height = (size_hints.height + 1) /2;

  Win = XCreateSimpleWindow(qtdisplay,qtwin,0,0,
			    size_hints.width,size_hints.height,
			    0,
			    BlackPixel(qtdisplay,XDefaultScreen(qtdisplay)),
			    WhitePixel(qtdisplay,XDefaultScreen(qtdisplay)));

			    */

  Win = XCreateSimpleWindow(qtdisplay,qtwin,1,1,
			    1,1,
			    0,
			    BlackPixel(qtdisplay,XDefaultScreen(qtdisplay)),
			    WhitePixel(qtdisplay,XDefaultScreen(qtdisplay)));

  PaintGC = XCreateGC(qtdisplay, Win, 0L, (XGCValues *) NULL);
  XSetForeground(qtdisplay, PaintGC, BlackPixel(qtdisplay, XDefaultScreen(qtdisplay) ));
  XSetBackground(qtdisplay, PaintGC, WhitePixel(qtdisplay, XDefaultScreen(qtdisplay) ));
  XSetFunction(qtdisplay, PaintGC, GXcopy);
  WorkCursor = XCreateFontCursor(qtdisplay, XC_watch);
  //ReadyCursor = XCreateFontCursor(qtdisplay, XC_plus);
  ReadyCursor = XCreateFontCursor(qtdisplay, XC_hand2);
  MoveCursor = XCreateFontCursor(qtdisplay, XC_fleur);
  LRCursor = XCreateFontCursor(qtdisplay, XC_sb_h_double_arrow);
  UDCursor = XCreateFontCursor(qtdisplay, XC_sb_v_double_arrow);

  XSelectInput(qtdisplay, Win, Button2MotionMask | ButtonPressMask |
	       ButtonReleaseMask | ExposureMask | KeyPressMask |
	       SubstructureNotifyMask | LeaveWindowMask | OwnerGrabButtonMask |
	       StructureNotifyMask);

  XMapRaised(qtdisplay, Win);

  XDefineCursor(qtdisplay, Win, ReadyCursor);
  XFlush(qtdisplay);

  for (oz = 0; oz < MAXZOOM; oz++)
    Images[oz] = NULL;


  // setup oz the default zoom factor
  for (oz = 0; oz < MAXZOOM && zfactor > (1 << oz); oz++){
  }


}




void TopLevel::zoomin(){

  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;

  if (oz > 0) {
    Image = Images[--oz];
    zfactor >>= 1;

    Resize = Refresh = 1;
  }

  int i;

  if (Image == NULL) {
    for (i = oz; i && (Images[i] == NULL); i--)
      ;
    for (; i != oz; i++){
      Images[i+1] = ZoomImage(Images[i]);
      Image = Images[i+1];

      if(Images[i+1] == NULL){ // out of memory	
	Image = Images[i];
	break;
      }
    }
  }

  PaneWidth = Image->width;
  PaneHeight = Image->height;

  resizeView();
  putImage();

  uiUpdate();
}

void TopLevel::zoomout()
{
  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;

  if (oz < MAXZOOM && Image->width >= 64 && zfactor < 32) {
    Image = Images[++oz];
    zfactor <<= 1;

    Resize = Refresh = 1;

  }

  int i;

  if (Image == NULL) {
    for ( i = oz; i && (Images[i] == NULL); i--)
      ;
    for (; i != oz; i++){
      Images[i+1] = ZoomImage(Images[i]);
      Image = Images[i+1];

      if(Images[i+1] == NULL){ // out of memory	
	Image = Images[i];
	break;
      }
    }
  }

  PaneWidth = Image->width;
  PaneHeight = Image->height;

  resizeView();
  putImage();

  uiUpdate();
}

void TopLevel::handle_X_event(XEvent Event)
{
  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;

  if(!Image)
    return;

  if(!faxqtwin || !display_is_setup)
    return;

  bool putimage = false; // Do we actually have to write the image to the scree?

  do {
    switch(Event.type) {
    case MappingNotify:
      XRefreshKeyboardMapping((XMappingEvent *)(&Event));
		break;

    case LeaveNotify:
      /*      buttondown = false;
	XDefineCursor(qtdisplay, Win, ReadyCursor);
	XFlush(qtdisplay);*/
      break;
    case Expose:
      {

	if(Event.xexpose.count != 0)
	  break;
	
	if(!Image)
	  break;

	putimage = TRUE;
      }	
    break;

    case KeyPress:
      if (ExpectConfNotify &&
	  (Event.xkey.time < (Lasttime + PATIENCE)))
	break;
      Lasttime = Event.xkey.time;
      ExpectConfNotify = 0;
      switch(XKeycodeToKeysym(qtdisplay, Event.xkey.keycode, 0)) {
      case XK_Help:
      case XK_h:
	helptiff();
	break;
      case XK_m:
	mirrorPage();
	if (Event.xkey.state & ShiftMask)
	  TurnFollowing(TURN_M, thispage->next);
  	break;
      case XK_o:
	zoomout();
	break;

      case XK_i:
	zoomin();
	break;

      case XK_Up:
	ypos-= qwindow_height / 3;
	putimage = TRUE;
	break;
      case XK_Down:
	ypos+= qwindow_height / 3;
	putimage = TRUE;
	break;
      case XK_Left:
	xpos-= qwindow_width / 4;
	putimage = TRUE;
	break;
      case XK_Right:
	xpos+= qwindow_width / 4;
	putimage = TRUE;
	break;
      case XK_Home:
      case XK_R7:	
	if (Event.xkey.state & ShiftMask) {
	  thispage = firstpage;
	  newPage();
	  resizeView();
	  putImage();
	  break;
	}
	xpos= 0;
	ypos= 0;
	putImage();
	break;
      case XK_End:
      case XK_R13:
	if (Event.xkey.state & ShiftMask) {
	  thispage = lastpage;
	  newPage();
	  resizeView();
	  putImage();
	  break;
	}
	xpos= Image->width;
	ypos= Image->height;
	putImage();
	break;
      case XK_l:
	rotatePage();
	if (Event.xkey.state & ShiftMask)
	  TurnFollowing(TURN_L, thispage->next);
	break;
      case XK_p:
      case XK_minus:
      case XK_Prior:
      case XK_R9:
      case XK_BackSpace:
	prevPage();
	break;
      case XK_n:
      case XK_plus:
      case XK_space:
      case XK_Next:
      case XK_R15:
	nextPage();
	break;
      case XK_u:
	flipPage();
	if (Event.xkey.state & ShiftMask)
	  TurnFollowing(TURN_U, thispage->next);
	break;

      case XK_q:
	if (viewpage) {
	  thispage = viewpage;
	  viewpage = NULL;
	  newPage();
	  resizeView();
	  putImage();
	}

      }

      break;

    case ButtonPress:

      if (ExpectConfNotify && (Event.xbutton.time < (Lasttime + PATIENCE)))
	break;

      Lasttime = Event.xbutton.time;
      ExpectConfNotify = 0;


      switch (Event.xbutton.button) {

      case Button1:
	buttondown = true;

	switch (((Image->width > qwindow_width)<<1) |
		(Image->height > qwindow_height)) {
	case 0:
	  break;
	case 1:
	  XDefineCursor(qtdisplay, Win, UDCursor);
	  break;
	case 2:
	  XDefineCursor(qtdisplay, Win, LRCursor);
	  break;
	case 3:
	  XDefineCursor(qtdisplay, Win, MoveCursor);
	}

	XFlush(qtdisplay);
		offx = Event.xbutton.x;
		offy = Event.xbutton.y;
	break;

      }

      break;

    case MotionNotify:
      if(!buttondown)
	break;
      do {

	nx = Event.xmotion.x;
	ny = Event.xmotion.y;


      } while (XCheckTypedEvent(qtdisplay, MotionNotify, &Event));


      xpos+= offx - nx;
      ypos+= offy - ny;

      offx = nx;
      offy = ny;

      putimage = TRUE;

      break;

    case ButtonRelease:

      if (Event.xbutton.button == Button1) {

	buttondown = false;
	XDefineCursor(qtdisplay, Win, ReadyCursor);
	XFlush(qtdisplay);
      }

    }

  } while (XCheckWindowEvent(qtdisplay, Win, KeyPressMask|ButtonPressMask, &Event));

  if(putimage == TRUE) {
    Refresh = Resize = 1;
    putImage();
  }
}

void TopLevel::rotatePage()
{
  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;

  XImage *newrotimage = NULL;

  XDefineCursor(qtdisplay, Win, WorkCursor);
  XFlush(qtdisplay);

  newrotimage = RotImage(Images[0]);

  if(newrotimage == NULL){ // out of memory
    XDefineCursor(qtdisplay, Win, ReadyCursor);
    return;
  }

  thispage->extra = Image = newrotimage;
  thispage->orient ^= TURN_L;

  int i;
  for (i = 1; Images[i]; i++) {
    FreeImage(Images[i]);
    Images[i] = NULL;
  }

  Images[0] = Image;

  for (i = 1; i <= oz; i++){

    Images[i] = ZoomImage(Images[i-1]);
    Image = Images[i];
    if(Images[i] == NULL){// out of memory
      Image = Images[i -1];
      break;
    }
  }


  { int t = xpos ; xpos= ypos; ypos= t; }

  Refresh = Resize = 1;

  //	XDefineCursor(qtdisplay, Win, ReadyCursor);

  putImage();
}

void TopLevel::flipPage()
{
  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;

  XImage *newflipimage = NULL;

  XDefineCursor(qtdisplay, Win, WorkCursor);
  XFlush(qtdisplay);

  newflipimage = FlipImage(Images[0]);

  if(newflipimage == NULL){ // out of memory
    XDefineCursor(qtdisplay, Win, ReadyCursor);
    return;
  }

  thispage->extra = Images[0] = newflipimage;
  thispage->orient ^= TURN_U;

  int i;
  for (i = 1; Images[i]; i++) {
    FreeImage(Images[i]);
    Images[i] = ZoomImage(Images[i-1]);
    if(Images[i]== NULL){// out of memory
      break;
    }
  }

  Image = Images[min(i-1,oz)];



/*
  XPutImage(qtdisplay, Win, PaintGC, Image,
  xpos -PaneWidth/2, ypos -PaneHeight/2,
  0, 0, PaneWidth, PaneHeight);

  XDefineCursor(qtdisplay, Win, ReadyCursor);
  */

  Refresh = Resize = 1;
  putImage();
}

void TopLevel::mirrorPage(){

  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;


  XImage *newmirror = NULL;

  XDefineCursor(qtdisplay, Win, WorkCursor);
  XFlush(qtdisplay);

  newmirror = MirrorImage(Images[0]);

  if(newmirror == NULL){ // out of memory
    XDefineCursor(qtdisplay, Win, ReadyCursor);
    return;
  }
  thispage->extra = Images[0] = newmirror;

  thispage->orient ^= TURN_M;

  int i;
  for (i = 1; Images[i]; i++) {
    FreeImage(Images[i]);
    Images[i] = ZoomImage(Images[i-1]);
    if (Images[i] == NULL) // out of memory
      break;
  }
  Image = Images[min(oz,i-1)];

  /*	XPutImage(qtdisplay, Win, PaintGC, Image,
	xpos - PaneWidth/2, ypos - PaneHeight/2,
	0, 0, PaneWidth, PaneHeight);
	XDefineCursor(qtdisplay, Win, ReadyCursor);*/

    Refresh = Resize = 1;
    putImage();


}

void TopLevel::scrollHorz(int){


  if(!Image)
    return;

  //  printf("hsb value: %d\n",hsb->value());
  xpos=  hsb->value() + qwindow_width/2;

  Refresh = 1;
  putImage();

}

void TopLevel::scrollVert(int ){

  if(!Image)
    return;

  //  printf("vsb value: %d\n",vsb->value());
  ypos=  vsb->value() + qwindow_height/2;


  Refresh = 1;
  putImage();
}

void TopLevel::helptiff(){
  if(!thispage){
    KMessageBox::sorry(this, i18n("You need to open a fax page first."));
    return;
  }

  if( viewpage ) // we are already displaying the help page
    return;

  if (helppage == NULL) {
    if (!notetiff( QFile::encodeName(locate("toolbar", "kfax.tif")) )) {
      return;
    }
    else {

      helppage = lastpage;
      lastpage = helppage->prev;
      lastpage->next = helppage->prev = NULL;

    }
  }
  viewpage = thispage;
  thispage = helppage;

  newPage();
  resizeView();
  putImage();

}
void TopLevel::goToEnd(){

  bool have_new_page = FALSE;

  if(!thispage)
    return;

  while(thispage->next != NULL) {

    have_new_page = TRUE;
    thispage = thispage->next;

  }

  if(have_new_page == TRUE){
    newPage();
    resizeView();
    putImage();
  }
  else
    KNotifyClient::beep(i18n("There are no more pages."));

}
void TopLevel::goToStart(){

  bool have_new_page = FALSE;

  if(!thispage)
    return;


  while(thispage->prev != NULL) {

    have_new_page = TRUE;
    thispage = thispage->prev;

  }

  if(have_new_page == TRUE){
    newPage();
    resizeView();
    putImage();
  }
  else
    KNotifyClient::beep(i18n("You are already on the first page."));

}
void TopLevel::nextPage(){

  if(!thispage)
    return;

  if (thispage->next == NULL) {

    KNotifyClient::beep(i18n("There is no further page."));
    return;
  }

  thispage = thispage->next;

  newPage();
  resizeView();
  putImage();
}

void TopLevel::prevPage(){

  if(!thispage)
    return;

  if (thispage->prev == NULL){

    KNotifyClient::beep(i18n("There is no previous page."));
    return;

  }

  thispage = thispage->prev;

  newPage();
  resizeView();
  putImage();

}

void TopLevel::newPage(){


  if(!display_is_setup)
    SetupDisplay();


  XDefineCursor(qtdisplay, Win, WorkCursor);
  XFlush(qtdisplay);


  int i;

  for (i = 1; Images[i]; i++) {
    FreeImage(Images[i]);
    Images[i] = NULL;
  }

  int k = -1;


  if(!thispage) {
    XDefineCursor(qtdisplay, Win, ReadyCursor);
    return;
  }

  if (Pimage(thispage) == NULL){

    while((k != 0) && (k != 3) && (k !=1))
      k = GetImage(thispage);

  }

  if (k == 3 ){

    XDefineCursor(qtdisplay, Win, ReadyCursor);
    FreeFax();
    /*    KMessageBox::sorry(i18n("Bad Fax File k=3"));*/
    return;
  }

  if (k == 0 ){

    XDefineCursor(qtdisplay, Win, ReadyCursor);
    FreeFax();
    /*    KMessageBox::sorry(i18n("Bad Fax File k=0"));*/
    return;
 }

  Image =  Images[0] = Pimage(thispage);

  setfaxtitle(thispage->name);

  for (i = 1; i <= oz; i++){
    Images[i] = ZoomImage(Images[i-1]);
    Image = Images[i];
    if (Images[i] == NULL){ // out of memory
      Image = Images[i-1];
      break;
    }
  }

  PaneWidth = Image->width;
  PaneHeight = Image->height;
  Refresh = 1;

  XDefineCursor(qtdisplay, Win, ReadyCursor);
  uiUpdate();

}


void TopLevel::closeFax()
{
  FreeFax();

  setCaption(i18n("KFax"));
  // TODO replace this with unmapping the window.
  if(display_is_setup)
    XResizeWindow(qtdisplay,Win,1,1); // we want a clear gray background.

  resizeView();
  vsb->hide();
  hsb->hide();
}



void TopLevel::FreeFax(){


  if(display_is_setup)
    XClearWindow(qtdisplay, Win);

  for (int i = 1; Images[i]; i++) {
	  FreeImage(Images[i]);
	  Images[i] = NULL;
  }

  pagenode *pn;

  for (pn = firstpage; pn; pn = pn->next){
    if(Pimage(pn)){
       FreeImage(Pimage(pn));
    }
  }

  Image = NULL;

  for (pn = firstpage; pn; pn = pn->next){
    if(pn->pathname){
       free(pn->pathname);
    }
  }


  if(firstpage){
    for(pn = firstpage->next; pn; pn = pn->next){
      if(pn->prev){
	free(pn->prev);
      }
    }	
  }

  if(lastpage)
    free(lastpage);

  // TODO what about the help page? Well it'll be freed when kfax exits
  firstpage = lastpage = viewpage = helppage = thispage = auxpage = NULL;

  uiUpdate();

}


void TopLevel::uiUpdate(){


  if(!thispage){

    toolBar()->setItemEnabled( ID_ZOOM_IN, FALSE );
    toolBar()->setItemEnabled( ID_ZOOM_OUT, FALSE );
    toolBar()->setItemEnabled( ID_PREV, FALSE );
    toolBar()->setItemEnabled( ID_START, FALSE );
    toolBar()->setItemEnabled( ID_NEXT, FALSE );
    toolBar()->setItemEnabled( ID_END, FALSE );
    toolBar()->setItemEnabled( ID_PRINT,FALSE);

    statusBar()->changeItem("", ID_INS_OVR);
    statusBar()->changeItem("", ID_GENERAL);
    statusBar()->changeItem("", ID_PAGE_NO);
    statusBar()->changeItem("",ID_FNAME);
    statusBar()->changeItem("",ID_TYPE);

    return;
  }

  if(  thispage->next != NULL){

    toolBar()->setItemEnabled( ID_NEXT, TRUE );
    toolBar()->setItemEnabled( ID_END, TRUE );

  }
  else{

    toolBar()->setItemEnabled( ID_NEXT, FALSE );
    toolBar()->setItemEnabled( ID_END, FALSE );

  }


  if(  thispage->prev != NULL){

    toolBar()->setItemEnabled( ID_PREV, TRUE );
    toolBar()->setItemEnabled( ID_START, TRUE );

  }
  else{

    toolBar()->setItemEnabled( ID_PREV, FALSE );
    toolBar()->setItemEnabled( ID_START, FALSE );

  }



  if(thispage){

    struct pagenode *pn ;
    int pages = 0;
    int currentpage = 0;

    for(pn = firstpage; pn ; pn = pn->next){
      pages ++;
      if (thispage == pn)
	currentpage = pages;
    }

    QString pagestr = i18n("P.%1 of %2").arg(currentpage).arg(pages);

    statusBar()->changeItem(pagestr, ID_PAGE_NO);

    toolBar()->setItemEnabled( ID_ZOOM_IN, TRUE );
    toolBar()->setItemEnabled( ID_ZOOM_OUT, TRUE );
    toolBar()->setItemEnabled( ID_PRINT,TRUE);
    if(Image){
      QString wh = i18n("W: %1 H: %2").arg(Image->width).arg(Image->height);
      statusBar()->changeItem(wh, ID_INS_OVR);
    }

    QString resolution = i18n("Res: %1").arg(thispage->vres?i18n("Fine"):i18n("Normal"));
    statusBar()->changeItem(resolution, ID_GENERAL);

    statusBar()->changeItem(thispage->name,ID_FNAME);

    QString typestring;

    if(thispage->type == FAX_TIFF){
      typestring = i18n("Type: Tiff ");
    }	
    else if ( thispage->type == FAX_RAW){
      typestring = i18n("Type: Raw ");
    }

    if ( thispage->expander == g31expand )
      typestring += "G3";

    if ( thispage->expander == g32expand )
      typestring += "G3 2D";

    if ( thispage->expander == g4expand )
      typestring += "G4";

    statusBar()->changeItem(typestring,ID_TYPE);
    //    QString memstr = i18n("Mem: %1").arg(Memused);
    //    statusBar()->changeItem(memstr, ID_LINE_COLUMN);

  }

}

void kfaxerror(const QString& title, const QString& error){
  KMessageBox::error(0, error, title);
}

void TopLevel::putImage(){

  // TODO do I really need to set Refresh or Resize to 1 , is there
  // really still someonce calling this with out haveing set Refresh or Resize to 1?

  if(!Image)
    return;

  if(!display_is_setup)
    return;

  if(!thispage)
    return;


  if ( qwindow_width > Image->width){
	  xpos= Image->width/2;
  }
  else{
    if(xpos< qwindow_width/2){
      xpos = qwindow_width/2;
    }
    else{
      if(xpos> Image->width - qwindow_width/2){
	xpos= Image->width - qwindow_width/2;
      }

    }
  }

  if ( qwindow_height > Image->height){
    ypos= Image->height/2;
  }
  else{
    if(ypos< qwindow_height/2){
      ypos = qwindow_height/2;
    }
    else{
      if(ypos> Image->height - qwindow_height/2){
	ypos= Image->height - qwindow_height/2;
      }

    }
  }

  if (xpos!= ox || ypos!= oy || Refresh || Resize){

    /* In the following we use qwindow_height -1 etc since the main view
       has a sunken frame and I need to offset by 1 pixel to the right and
       one pixel down so that I don't paint my fax into the border of the frame*/

    XResizeWindow(qtdisplay,Win,min(qwindow_width -1,Image->width ),
		  min(qwindow_height -1,Image->height ));

    XPutImage(qtdisplay, Win, PaintGC, Image,
	      max(xpos - qwindow_width/2,0), max(ypos - qwindow_height/2,0),
	      0, 0, min(qwindow_width -1,Image->width)  ,
	      min(qwindow_height -1,Image->height) );

    vsb->setValue(max(ypos - qwindow_height/2,0));
    hsb->setValue(max(xpos - qwindow_width/2,0));

    XFlush(qtdisplay);
  }

  ox = xpos;
  oy = ypos;

  Resize = Refresh = 0;

}


void TopLevel::faxoptions(){

 OptionsDialog * opd = new OptionsDialog(NULL,"options");
 opd->setWidgets(&fop);

 if(opd->exec()){

   struct optionsinfo *newops;
   newops = opd->getInfo();

   fop.resauto 	=  newops->resauto;
   fop.geomauto	=  newops->geomauto;
   fop.width 	=  newops->width;
   fop.height 	=  newops->height;
   fop.fine 	=  newops->fine;
   fop.landscape=  newops->landscape;
   fop.flip 	=  newops->flip;
   fop.invert 	=  newops->invert;
   fop.lsbfirst =  newops->lsbfirst;
   fop.raw 	=  newops->raw;


   setFaxDefaults();

 }

 delete opd;

}

void setFaxDefaults(){

  // fop is called in readSettings, so this can't be
  // called after a TopLevel::readSettings()

  if(have_cmd_opt ) // we have commad line options all kfaxrc defaults are
    return;         // overridden

  if(fop.resauto == 1){
    defaultpage.vres = -1;
  }
  else{
    defaultpage.vres = fop.fine;
  }

  if(fop.geomauto == 1){
    defaultpage.width = 0;
    defaultpage.height = 0;
  }
  else{
    defaultpage.width = fop.width;
    defaultpage.height = fop.height;
  }

  if( fop.landscape)
    defaultpage.orient |= TURN_L;

  if(fop.flip)
     defaultpage.orient |= TURN_U;

  defaultpage.inverse =    fop.invert;
  defaultpage.lsbfirst = fop.lsbfirst;

  if(fop.raw == 2)
    defaultpage.expander = g32expand;
  if(fop.raw == 4)
    defaultpage.expander = g4expand;
  if((fop.raw != 4) && (fop.raw != 2) )
    defaultpage.expander = g31expand;

}

static const char *description =
        I18N_NOOP("KDE G3/G4 Fax Viewer");

int main (int argc, char **argv)
{
  toplevel = NULL;

  catchSignals();

  KAboutData aboutData( "kfax", I18N_NOOP("KFax"),
      KFAXVERSION, description, KAboutData::License_GPL,
      "(c) 1997-98 Bernd Johannes Wuebben");
  aboutData.addAuthor( "Bernd Johannes Wuebben", 0, "wuebben@kde.org" );

  KCmdLineArgs::init(argc, argv, &aboutData);

  viewfax_addCmdLineOptions();

  MyApp a;

  qtdisplay = qt_xdisplay();

  viewfaxmain();

  toplevel = new TopLevel();
  toplevel->show ();

  startingup = 1;
  a.processEvents();
  a.flushX();

  startingup = 0;

  faxinit();

  if(!have_no_fax){

    thispage = firstpage;

    toplevel->newPage();
    toplevel->resizeView();
    //TODO : I don't think I need this putImage();
    toplevel->putImage();
  }

  toplevel->uiUpdate();

  return a.exec ();
}

void setfaxtitle(const QString& name){

  toplevel->setFaxTitle(name);

}

void setstatusbarmem(int mem){

  if(toplevel)
    toplevel->setStatusBarMemField(mem);

}


void mysighandler(int ){

  //  printf("signal received %d\n",sig);
  catchSignals(); // reinstall signal handler

}


void catchSignals()
{
  /*
	signal(SIGHUP, mysighandler);	
	signal(SIGINT, mysighandler);	
	signal(SIGTERM, mysighandler);	
	signal(SIGCHLD, mysighandler);
  	signal(SIGABRT, mysighandler);
	signal(SIGUSR1, mysighandler);
	signal(SIGALRM, mysighandler);
	signal(SIGFPE, mysighandler);
	signal(SIGILL, mysighandler);*/

	signal(SIGPIPE, mysighandler);

/*	signal(SIGQUIT, mysighandler);
	signal(SIGSEGV, mysighandler);

#ifdef SIGBUS
	signal(SIGBUS, mysighandler);
#endif
#ifdef SIGPOLL
	signal(SIGPOLL, mysighandler);
#endif
#ifdef SIGSYS
	signal(SIGSYS, mysighandler);
#endif
#ifdef SIGTRAP
	signal(SIGTRAP, mysighandler);
#endif
#ifdef SIGVTALRM
	signal(SIGVTALRM, mysighandler);
#endif
#ifdef SIGXCPU
	signal(SIGXCPU, mysighandler);
#endif
#ifdef SIGXFSZ
	signal(SIGXFSZ, mysighandler);
#endif
*/

}



void parse(char* buf, char** args){

  while(*buf != '\0'){

    // Strip whitespace. Use nulls, so that the previous argument is terminated
    // automatically.

    while ((*buf == ' ' ) || (*buf == '\t' ) || (*buf == '\n' ) )
      *buf++ ='\0';

    // save the argument
    if(*buf != '\0')
    *args++ = buf;

    while ((*buf != '\0') && (*buf != '\n') && (*buf != '\t') && (*buf != ' '))
      buf++;

  }

  *args ='\0';;

}

int copyfile(const char* toname,char* fromname)
{
  char buffer[4*1028];
  int count = 0;
  int count2;

  FILE*  fromfile;
  FILE*  tofile;

  if (QFile::exists(toname)) {
    if(KMessageBox::questionYesNo( 0,
			      i18n("A file with this name already exists\n"
			      "Do you want to overwrite it?\n\n")))
      return 1;
  }

  if((fromfile = fopen(fromname,"r")) == NULL)
    return 0;

  if((tofile =  fopen(toname,"w")) == NULL){
    fclose(fromfile);
    return 0;
  }


  while((count = fread(buffer,sizeof(char),4*1028,fromfile))){
    count2 = fwrite(buffer,sizeof(char),count,tofile);
    if (count2 != count){
      fclose(fromfile);
      fclose(tofile);
      return 0;
    }

  }
  fclose(fromfile);
  fclose(tofile);

  return 1;
}
