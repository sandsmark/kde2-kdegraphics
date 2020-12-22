#include <stdlib.h>

#include <qfileinfo.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qscrollbar.h>
#include <qtimer.h>
#include <qprinter.h>
#include <qprintdialog.h>
#include <qobjectlist.h>


#include <kaccel.h>
#include <kdebug.h>
#include <kinstance.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kio/job.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kconfig.h>

#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "marklist.h"
#include "scrollbox.h"
#include "gotodialog.h"
#include "kpagetest.h"
#include "kviewpart.moc"


extern "C"
{
  void *init_libkviewerpart()
  {
    return new KViewPartFactory;
  }
};


KInstance *KViewPartFactory::s_instance = 0L;


KViewPartFactory::KViewPartFactory()
{
}


KViewPartFactory::~KViewPartFactory()
{
  if (s_instance)
    delete s_instance;

  s_instance = 0;
}


KParts::Part *KViewPartFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *classname, const QStringList &args )
{
  QString partname = "";
  if (args.count() >= 1)
    partname = args[0];
  KParts::Part *obj = new KViewPart(partname, parentWidget, widgetName, parent, name);
  emit objectCreated(obj);
  return obj;
}


KInstance *KViewPartFactory::instance()
{
  if (!s_instance)
    s_instance = new KInstance("kviewerpart");
  return s_instance;
}


class PaperSize
{
public:

  PaperSize(double w, double h)
    : width(w), height(h) {};

  double width;
  double height;
};


KViewPart::KViewPart(QString partname, QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name)
  : KParts::ReadOnlyPart(parent, name), _partname(partname), _numberOfPages(0),
  _currentPage(0), _zoom(1.0)
{
  setInstance(KViewPartFactory::instance());

  mainWidget = new QWidget(parentWidget, widgetName);
  mainWidget->setFocusPolicy(QWidget::StrongFocus);
  setWidget(mainWidget);

  QHBoxLayout *hbox = new QHBoxLayout(mainWidget, 0, 0);
  QVBoxLayout *vbox = new QVBoxLayout(hbox);

  scrollBox = new ScrollBox(mainWidget);
  scrollBox->setFixedWidth(75);
  scrollBox->setMinimumHeight(75);
  vbox->addWidget(scrollBox);

  connect(scrollBox, SIGNAL(valueChanged(QPoint)), this, SLOT(scrollBoxChanged(QPoint)));

  markList = new MarkList(mainWidget);
  markList->setAutoUpdate(true);
  vbox->addWidget(markList);
  vbox->setStretchFactor(markList, 1);

  connect(markList, SIGNAL(selected(int)), this, SLOT(pageSelected(int)));

  // create the displaying part

  // Try to load
  KLibFactory *factory = KLibLoader::self()->factory(QString("libk%1").arg(_partname).utf8());
  if (factory) {
    multiPage = (KMultiPage*) factory->create(mainWidget, QString("k%1").arg(_partname).utf8(), "KPart");
  } else {
    KMessageBox::error(mainWidget, QString("No libk%1 found !").arg(_partname));
    ::exit(-1);
  }
  hbox->addWidget(multiPage->widget());

  // connect to the multi page view
  connect(multiPage, SIGNAL(numberOfPages(int)), this, SLOT(numberOfPages(int)));
  connect(multiPage, SIGNAL(pageInfo(int, int)), this, SLOT(pageInfo(int, int)) );
  connect(multiPage->scrollView(), SIGNAL(contentsMoving(int,int)), this, SLOT(contentsMoving(int,int)));
  multiPage->scrollView()->installEventFilter(this);
  connect( multiPage, SIGNAL( started( KIO::Job * ) ), this, SIGNAL( started( KIO::Job * ) ) );
  connect( multiPage, SIGNAL( completed() ), this, SIGNAL( completed() ) );
  connect( multiPage, SIGNAL( canceled( const QString & ) ), this, SIGNAL( canceled( const QString & ) ) );
  connect( multiPage, SIGNAL( previewChanged(bool)), this, SLOT(updatePreview(bool)));
  connect( multiPage, SIGNAL( setStatusBarText( const QString& ) ), this, SIGNAL( setStatusBarText( const QString& ) ) );

  // settings menu
  showmarklist = new KToggleAction (i18n("Show &Page List"), 0, this, SLOT(slotShowMarkList()), actionCollection(), "show_page_list");
  showPreview = new KToggleAction (i18n("Show P&review"), 0, this, SLOT(slotPreview()), actionCollection(), "show_preview");
  watchAct = new KToggleAction(i18n("&Watch file"), 0, 0, 0, actionCollection(), "watch_file");

  // view menu
  QStringList orientations;
  orientations.append(i18n("Portrait"));
  orientations.append(i18n("Landscape"));
  orientation = new KSelectAction (i18n("&Orientation"), 0, 0, 0, actionCollection(), "view_orientation");
  connect (orientation, SIGNAL(activated (int)), this, SLOT(slotOrientation(int)));
  orientation->setItems(orientations);

  media = new KSelectAction (i18n("Paper &Size"), 0, 0, 0, actionCollection(), "view_media");
  connect (media, SIGNAL(activated(int)), this, SLOT(slotMedia(int)));
  fillPaperSizes();

  _zoom        = 1.0;
  _paperWidth  = 21.0;
  _paperHeight = 29.7;
  setPaperSize(_paperWidth, _paperHeight);

  zoomInAct = KStdAction::zoomIn (this, SLOT(zoomIn()), actionCollection());
  zoomOutAct = KStdAction::zoomOut(this, SLOT(zoomOut()), actionCollection());

  fitAct = KStdAction::actualSize(this, SLOT(fitSize()), actionCollection());
  fitPageAct = KStdAction::fitToPage(this, SLOT(fitToPage()), actionCollection());
  fitWidthAct = KStdAction::fitToWidth(this, SLOT(fitToWidth()), actionCollection());
  fitHeightAct = KStdAction::fitToHeight(this, SLOT(fitToHeight()), actionCollection());

  // go menu
  backAct = KStdAction::prior(this, SLOT(prevPage()), actionCollection());
  forwardAct = KStdAction::next(this, SLOT(nextPage()), actionCollection());
  startAct = KStdAction::firstPage(this, SLOT(firstPage()), actionCollection());
  endAct = KStdAction::lastPage(this, SLOT(lastPage()), actionCollection());
  gotoAct = KStdAction::gotoPage(this, SLOT(goToPage()), actionCollection());

  readDownAct = new KAction(i18n("Read down document"), "next",
			    Key_Space, this, SLOT(readDown() ),
			    actionCollection(), "go_read_down");
  /*
    markAct =
      new KAction(i18n("Toggle this page mark"),
		  QIconSet(BarIcon("flag", KGVFactory::instance())),
		  Key_Enter, w, SLOT(markPage() ), actionCollection(), "markPage");
    */

    //TODO -- disable entry if there aren't any page names
    //Settings menu
    /*
    fancyAct =
      new KToggleAction (i18n("Show Page Names"),
			 0, this, SLOT(slotFancy()), actionCollection(),
			 "fancy_page_labels");
    fancyAct->setChecked (w->areFancyPageLabelsEnabled ());
    */

  printAction = KStdAction::print(this, SLOT(slotPrint()), actionCollection());
  saveAction = KStdAction::saveAs(multiPage, SLOT(slotSave()), actionCollection());

  // keyboard accelerators
  accel = new KAccel(mainWidget);
  accel->insertItem(i18n("Scroll Up"), "Scroll Up", "Up");
  accel->insertItem(i18n("Scroll Down"), "Scroll Down", "Down");
  accel->insertItem(i18n("Scroll Left"), "Scroll Left", "Left");
  accel->insertItem(i18n("Scroll Right"), "Scroll Right", "Right");

  accel->insertItem(i18n("Scroll Up Page"), "Scroll Up Page", "Shift+Up");
  accel->insertItem(i18n("Scroll Down Page"), "Scroll Down Page", "Shift+Down");
  accel->insertItem(i18n("Scroll Left Page"), "Scroll Left Page", "Shift+Left");
  accel->insertItem(i18n("Scroll Right Page"), "Scroll Right Page", "Shift+Right");

  accel->readSettings();

  accel->connectItem("Scroll Up", this, SLOT(scrollUp()));
  accel->connectItem("Scroll Down", this, SLOT(scrollDown()));
  accel->connectItem("Scroll Left", this, SLOT(scrollLeft()));
  accel->connectItem("Scroll Right", this, SLOT(scrollRight()));

  accel->connectItem("Scroll Up Page", this, SLOT(scrollUpPage()));
  accel->connectItem("Scroll Down Page", this, SLOT(scrollDownPage()));
  accel->connectItem("Scroll Left Page", this, SLOT(scrollLeftPage()));
  accel->connectItem("Scroll Right Page", this, SLOT(scrollRightPage()));

  setXMLFile("kviewerpart.rc");


  connect(&watch, SIGNAL(dirty(const QString&)), this, SLOT(fileChanged(const QString&)));


  m_extension = new KViewPartExtension(this);

  // create the goto dialog
  _gotoDialog = new GotoDialog(mainWidget);
  _gotoDialog->hide();
  connect(_gotoDialog, SIGNAL(gotoPage(const QString&)), this, SLOT(slotGotoDialog(const QString&)));

  numberOfPages(0);
  checkActions();

  // allow parts to have a GUI, too :-)
  // (will be merged automatically)
  insertChildClient( multiPage );

  orientation->setCurrentItem(0);
  media->setCurrentItem(1);

  readSettings();

  // watch mouse events in the viewport
  QWidget *vp = multiPage->scrollView()->viewport();
  vp->installEventFilter(this);
  // we also have to set an filter on the child
  // TODO: solve this more elegant
  if (vp->children()) {
    QWidget *w = (QWidget*)((QObjectList*)(vp->children()))->first();
    w->installEventFilter(this);
  }
}


KViewPart::~KViewPart()
{
  writeSettings();
  delete multiPage;
}


void KViewPart::fillPaperSizes()
{
  _paperSizes.clear();

  QString fname = KGlobal::dirs()->findResource("data", "kviewshell/paper-formats");
  kdDebug() << "Paper: " << fname << endl;

  KConfig paper(fname, true);
  paper.setGroup("Paper");
  unsigned int count = paper.readNumEntry("Count");

  double w, h;
  for (unsigned int i=0; i<count; i++)
    {
      paper.setGroup(QString("Paper %1").arg(i));
      w = paper.readDoubleNumEntry("Width");
      h = paper.readDoubleNumEntry("Height");
      _paperSizes.append(new PaperSize(w,h));
    }

  QStringList items;
  items << i18n("A3");
  items << i18n("A4");
  items << i18n("A5");
  items << i18n("Letter");
  items << i18n("Legal");
  items << i18n("Other...");
  media->setItems(items);
}


void KViewPart::setPaperSize(double w, double h)
{
  // reflect in scrollbox
  scrollBox->setMinimumHeight( int(75.0*h/w+0.5));

  // forward
  multiPage->setPaperSize(w, h);

  updateScrollBox();
}


QStringList KViewPart::fileFormats()
{
  return multiPage->fileFormats();
}


void KViewPart::slotShowMarkList()
{
  if (showmarklist->isChecked()) {
    markList->show();
    scrollBox->show();
  } else {
    markList->hide();
    scrollBox->hide();
  }
}

bool KViewPart::openFile()
{
  KURL tmpFileURL;

  // We try to be error-tolerant about filenames. If the user calls us
  // with something like "test", and we are using the DVI-part, we'll
  // also look for "testdvi" and "test.dvi".
  QFileInfo fi(m_file);
  if (fi.exists() == false) {
    // First produce a list of possible endings we try to append to
    // the filename. The decompose the format strings like "*.dvi
    // *.DVI|DVI File" coming from the multipage part.
    QStringList endings;
    QStringList formats = multiPage->fileFormats();
    for ( QStringList::Iterator it = formats.begin(); it != formats.end(); ++it ) {
      QString ending = (*it).simplifyWhiteSpace();
      int bar = ending.find('|');
      if (bar != -1)
	ending = ending.left(bar);

      QStringList localendings = QStringList::split(" ",ending);
      for ( QStringList::Iterator it2 = localendings.begin(); it2 != localendings.end(); ++it2 )
	endings += (*it2).mid(2); // remove "*." of "*.dvi"
    }

    // Now try to append the endings with and without dots, and see if
    // that gives a file.
    for ( QStringList::Iterator it = endings.begin(); it != endings.end(); ++it ) {
      fi.setFile(m_file+(*it));
      if (fi.exists() == true) {
	m_file = m_file+(*it);
	break;
      } else {
	fi.setFile(m_file+"."+(*it));
	if (fi.exists() == true) {
	  m_file = m_file+"."+(*it);
	  break;
	}
      }
    }
    // Set a new window caption is we modified the file name.
    tmpFileURL.setPath(m_file);
    emit setWindowCaption( tmpFileURL.prettyURL() );
  }

  // Now call the openURL-method of the multipage and give it an URL
  // pointing to the downloaded file.
  tmpFileURL.setPath(m_file);
  bool r = multiPage->openURL(tmpFileURL);

  updateScrollBox();
  markList->select(0);

  if (r) {
    // start viewing horizontally centered
    QScrollView *sv = multiPage->scrollView();
    if (sv)
      sv->center(sv->contentsWidth()/2, 0);
    // Note: KDirWatch is able to watch files, as well.
    // Hope this stays that way...
    watch.addDir(m_file);
    watch.startScan();
  } else {
    m_url = "";
    emit setWindowCaption( "" );
  }

  checkActions();
  return r;
}


void KViewPart::fileChanged(const QString &file)
{
  kdDebug() << "File changed: " << file << endl;

  if (file == m_file && watchAct->isChecked())
    multiPage->reload();
}


bool KViewPart::closeURL()
{
  watch.removeDir(m_file);
  watch.stopScan();

  KParts::ReadOnlyPart::closeURL();

  multiPage->closeURL();

  m_url = "";

  numberOfPages(0);
  checkActions();

  emit setWindowCaption("");

  return true;
}


void KViewPart::slotOrientation(int id)
{
  if (id == 0)
    setPaperSize(_paperWidth, _paperHeight);
  else
    setPaperSize(_paperHeight, _paperWidth);
}


void KViewPart::slotMedia(int id)
{
  double w=0.0, h=0.0;

  // This hard-coded list is really a shame. We should use a more
  // flexible global list here. If you modify here, don't forget to
  // modify the hard-coded list in readSettings() as well!
  switch (id) {
  case 0:
    w = 29.7; h = 42.0;
    break;
  case 1:
    w = 21.0; h = 29.7;
    break;
  case 2:
    w = 14.85; h = 21.0;
    break;
  case 3:
    w = 21.59; h = 27.94;
    break;
  case 4:
    w = 21.50; h = 35.56;
    break;
  case 5:
    // TODO : paper size dialog!
    break;
  }

  if (w != 0.0 && h != 0.0)
    if (orientation->currentItem() == 0)
      setPaperSize(w, h);
    else
      setPaperSize(h, w);
  _paperWidth = w;
  _paperHeight = h;
}


void KViewPart::numberOfPages(int nr)
{
  _numberOfPages = nr;

  markList->clear();

  if (nr == 0) {
    // clear scroll box
    scrollBox->setPageSize(QSize(0,0));
    scrollBox->setViewSize(QSize(0,0));
    _currentPage = 0;
    return;
  }

  for (int i=0; i<nr; i++)
    markList->insertItem(QString("%1").arg(i+1), i);

  setPage(0);
}

void KViewPart::pageInfo(int numpages, int currentpage)
{
  _numberOfPages = numpages;

  markList->clear();

  if (numpages == 0) {
    // clear scroll box
    scrollBox->setPageSize(QSize(0,0));
    scrollBox->setViewSize(QSize(0,0));
    
    _currentPage = 0;

    return;
  }

  for (int i=0; i<numpages; i++)
    markList->insertItem(QString("%1").arg(i+1), i);

  _currentPage = currentpage;
  markList->select(currentpage);
  checkActions();
  updateScrollBox();
}


void KViewPart::setPage(int page)
{
  _currentPage = page;

  if (!multiPage->gotoPage(page))
    return;

  markList->select(page);

  checkActions();
  updateScrollBox();
}


void KViewPart::prevPage()
{
  if (page() > 0)
    setPage(page()-1);
}


void KViewPart::nextPage()
{
  if (page()+1 < pages())
    setPage(page()+1);
}


void KViewPart::firstPage()
{
  setPage(0);
}


void KViewPart::lastPage()
{
  setPage(pages()-1);
}


void KViewPart::goToPage()
{
  _gotoDialog->show();
}


void KViewPart::slotGotoDialog(const QString &page)
{
  bool ok;
  int p = page.toInt(&ok)-1;
  if (ok && p >= 0 && p < pages())
    setPage(p);
}


void KViewPart::pageSelected(int nr)
{
  if ((nr >= 0) && (nr<pages()))
    setPage(nr);
}


void KViewPart::zoomIn()
{
  _zoom = _zoom * pow(2.0, 1.0/4.0);
  if (_zoom < minZoom/1000.0)
    _zoom = minZoom/1000.0;
  if (_zoom > maxZoom/1000.0)
    _zoom = maxZoom/1000.0;
  _zoom = multiPage->setZoom(_zoom);
  updateScrollBox();
}


void KViewPart::zoomOut()
{
  _zoom = _zoom / pow(2.0, 1.0/4.0);
  if (_zoom < minZoom/1000.0)
    _zoom = minZoom/1000.0;
  if (_zoom > maxZoom/1000.0)
    _zoom = maxZoom/1000.0;
  _zoom = multiPage->setZoom(_zoom);
  updateScrollBox();
}


void KViewPart::fitToPage()
{
  double w, h;

  w = multiPage->zoomForWidth(pageSize().width());
  h = multiPage->zoomForHeight(pageSize().height());

  if (w < h)
    _zoom = w;
  else
    _zoom = h;
  if (_zoom < minZoom/1000.0)
    _zoom = minZoom/1000.0;
  if (_zoom > maxZoom/1000.0)
    _zoom = maxZoom/1000.0;
  _zoom = multiPage->setZoom(_zoom);
  updateScrollBox();
}


void KViewPart::fitSize()
{
  _zoom = multiPage->setZoom(1.0);
  updateScrollBox();
}


void KViewPart::fitToHeight()
{
  // See below, in the documentation of the method "fitToWidth" for an
  // explanation of the complicated calculation we are doing here.
  int targetHeight  = multiPage->scrollView()->viewportSize(0,0).height()-1;
  int targetWidth;
  if (orientation->currentItem() == 0)
    targetWidth = (int)(targetHeight * _paperWidth/_paperHeight +0.5);
  else
    targetWidth = (int)(targetHeight * _paperHeight/_paperWidth +0.5);
  targetHeight = multiPage->scrollView()->viewportSize(targetWidth, targetHeight).height()-1;
  
  _zoom = multiPage->zoomForHeight(targetHeight);
  if (_zoom < minZoom/1000.0)
    _zoom = minZoom/1000.0;
  if (_zoom > maxZoom/1000.0)
    _zoom = maxZoom/1000.0;
  _zoom = multiPage->setZoom(_zoom);
  updateScrollBox();
}


void KViewPart::fitToWidth()
{
  // There is a slight complication here... if we just take the width
  // of the viewport and scale the contents by a factor x so that it
  // fits the viewport exactly, then, depending on choosen papersize
  // (landscape, etc.), the contents may be higher than the viewport
  // and the QScrollview may or may not insert a scrollbar at the
  // right. If the scrollbar appears, then the usable width of the
  // viewport becomes smaller, and scaling by x does not really fit
  // the (now smaller page) anymore.

  // Caluculate the width and heigt of the view, disregarding the
  // possible complications with scrollbars, e.g. assuming the maximal
  // space is available.
  int targetWidth  = multiPage->scrollView()->viewportSize(0,0).width()-1;
  int targetHeight;
  if (orientation->currentItem() == 0)
    targetHeight = (int)(targetWidth * _paperHeight/_paperWidth +0.5);
  else
    targetHeight = (int)(targetWidth * _paperWidth/_paperHeight +0.5);
  // Think again, this time use only the area which is really
  // acessible (which, in case that targetWidth targetHeight don't fit
  // the viewport, is really smaller because of the scrollbars).
  targetWidth      = multiPage->scrollView()->viewportSize(targetWidth, targetHeight).width()-1;

  _zoom = multiPage->zoomForWidth(targetWidth);
  if (_zoom < minZoom/1000.0)
    _zoom = minZoom/1000.0;
  if (_zoom > maxZoom/1000.0)
    _zoom = maxZoom/1000.0;
  _zoom = multiPage->setZoom(_zoom);
  updateScrollBox();
}


QSize KViewPart::pageSize()
{
  QRect r = multiPage->widget()->childrenRect();
  return QSize(r.width(), r.height());
}


void KViewPart::updateScrollBox()
{
  QScrollView *sv = multiPage->scrollView();
  scrollBox->setPageSize(QSize(sv->contentsWidth(), sv->contentsHeight()));
  scrollBox->setViewSize(QSize(sv->visibleWidth(), sv->visibleHeight()));
  scrollBox->setViewPos(QPoint(sv->contentsX(), sv->contentsY()));
}


void KViewPart::checkActions()
{
  bool doc = !url().isEmpty();

  backAct->setEnabled(doc && page() > 0);
  forwardAct->setEnabled(doc && page()+1 < pages());
  startAct->setEnabled(doc && page() > 0);
  endAct->setEnabled(doc && page()+1 < pages());
  gotoAct->setEnabled(doc && pages()>1);

  zoomInAct->setEnabled(doc);
  zoomOutAct->setEnabled(doc);

  fitAct->setEnabled(doc);
  fitPageAct->setEnabled(doc);
  fitHeightAct->setEnabled(doc);
  fitWidthAct->setEnabled(doc);

  media->setEnabled(doc);
  orientation->setEnabled(doc);

  printAction->setEnabled(doc);
  saveAction->setEnabled(doc);
}


void KViewPart::contentsMoving(int x, int y)
{
  QScrollView *sv = multiPage->scrollView();
  scrollBox->setPageSize(QSize(sv->contentsWidth(), sv->contentsHeight()));
  scrollBox->setViewSize(QSize(sv->visibleWidth(), sv->visibleHeight()));
  scrollBox->setViewPos(QPoint(x,y));
}


void KViewPart::scrollBoxChanged(QPoint np)
{
  multiPage->scrollView()->setContentsPos(np.x(), np.y());
}


bool KViewPart::eventFilter(QObject *obj, QEvent *ev)
{
  if (obj == this && ev->type() == QEvent::Resize)
    QTimer::singleShot(0, this, SLOT(updateScrollBox()));

  if (obj != this) {
    if (ev->type() == QEvent::MouseButtonPress) {
      mousePos = ((QMouseEvent*)ev)->globalPos();
      multiPage->scrollView()->setCursor(Qt::sizeAllCursor);
    }

    if (ev->type() == QEvent::MouseMove) {
      QPoint newPos = ((QMouseEvent*)ev)->globalPos();
      if ( ((QMouseEvent*)ev)->state() == LeftButton ) {
	QPoint delta = mousePos - newPos;
	multiPage->scrollView()->scrollBy(delta.x(), delta.y());
      }
      mousePos = newPos;
    }
    
    if (ev->type() == QEvent::MouseButtonRelease)
      multiPage->scrollView()->setCursor(Qt::arrowCursor);
  }
  
  return false;
}


void KViewPart::scrollUp()
{
  QScrollBar *sb = multiPage->scrollView()->verticalScrollBar();
  if (sb)
    sb->subtractLine();
  updateScrollBox();
}


void KViewPart::scrollDown()
{
  QScrollBar *sb = multiPage->scrollView()->verticalScrollBar();
  if (sb)
    sb->addLine();
  updateScrollBox();
}


void KViewPart::scrollLeft()
{
  QScrollBar *sb = multiPage->scrollView()->horizontalScrollBar();
  if (sb)
    sb->subtractLine();
  updateScrollBox();
}


void KViewPart::scrollRight()
{
  QScrollBar *sb = multiPage->scrollView()->horizontalScrollBar();
  if (sb)
    sb->addLine();
  updateScrollBox();
}


void KViewPart::scrollUpPage()
{
  QScrollBar *sb = multiPage->scrollView()->verticalScrollBar();
  if (sb)
    sb->subtractPage();
  updateScrollBox();
}


void KViewPart::scrollDownPage()
{
  QScrollBar *sb = multiPage->scrollView()->verticalScrollBar();
  if (sb)
    sb->addPage();
  updateScrollBox();
}


void KViewPart::scrollLeftPage()
{
  QScrollBar *sb = multiPage->scrollView()->horizontalScrollBar();
  if (sb)
    sb->subtractPage();
  updateScrollBox();
}


void KViewPart::scrollRightPage()
{
  QScrollBar *sb = multiPage->scrollView()->horizontalScrollBar();
  if (sb)
    sb->addPage();
  updateScrollBox();
}


void KViewPart::scrollTo(int x, int y)
{
  QScrollView *sv = multiPage->scrollView();
  sv->setContentsPos(x, y);
  updateScrollBox();
}


void KViewPart::updatePreview(bool avail)
{
  if (_numberOfPages == 0)
    emit setStatusBarText("");
  else
    emit setStatusBarText(QString(i18n("Page %1 of %2")).arg(_currentPage+1).arg(_numberOfPages));


  QPixmap pixmap(scrollBox->width(), scrollBox->height());
  QPainter p(&pixmap);

  if (showPreview->isChecked() && avail
      && multiPage->preview(&p, scrollBox->width(), scrollBox->height()))
    scrollBox->setBackgroundPixmap(pixmap);
  else
    scrollBox->setBackgroundMode(QFrame::PaletteMid);
}


void KViewPart::slotPreview()
{
  updatePreview(true);
}


void KViewPart::slotPrint()
{
  QStringList pages;

  for (int i=0; i<markList->count(); i++)
    if (markList->isSelected(i))
      pages.append(QString::number(i));

  multiPage->print(pages, page());
}



void KViewPart::readSettings()
{
  KConfig *config = instance()->config();

  config->setGroup("GUI");

  showmarklist->setChecked(config->readBoolEntry("PageMarks", true));
  slotShowMarkList();

  watchAct->setChecked(config->readBoolEntry("WatchFile", true));
  showPreview->setChecked(config->readBoolEntry("ShowPreview", true));
  _zoom = config->readDoubleNumEntry("Zoom", 1.0);
  if ( (_zoom < minZoom/1000.0) || (_zoom > maxZoom/1000.0))
    _zoom = 1.0;
  _zoom = multiPage->setZoom(_zoom);

  // Read Paper Size and orientation
  int orient = config->readNumEntry("Orientation", 0 );
  orientation->setCurrentItem(orient);
  double w = config->readDoubleNumEntry("PaperWidth", 21.0 );
  double h = config->readDoubleNumEntry("PaperHeight", 29.7 );
  if (orient == 0)
    setPaperSize(w, h);
  else
    setPaperSize(h, w);

  // Try the find the suitable paper size. This hard-coded list is
  // really a shame. We should use a more flexible global list
  // here. By the way, it is never a good idea to check if two floats
  // are equal... here that is not a problem (yet) because we never do
  // any calculation.
  int mediaitem = 5;
  if ((w == 29.70) && (h == 42.00))
    mediaitem = 0;
  if ((w == 21.00) && (h == 29.70))
    mediaitem = 1;
  if ((w == 14.85) && (h == 21.00))
    mediaitem = 2;
  if ((w == 21.59) && (h == 27.94))
    mediaitem = 3;
  if ((w == 21.50) && (h == 35.56))
    mediaitem = 4;
  media->setCurrentItem(mediaitem);

}


void KViewPart::writeSettings()
{
  KConfig *config = instance()->config();

  config->setGroup("GUI");

  config->writeEntry("PageMarks", showmarklist->isChecked());
  config->writeEntry("WatchFile", watchAct->isChecked());
  config->writeEntry("ShowPreview", showPreview->isChecked());
  config->writeEntry("Zoom", _zoom);

  config->writeEntry("Orientation", orientation->currentItem() );
  config->writeEntry("PaperWidth",  _paperWidth );
  config->writeEntry("PaperHeight", _paperHeight );
  config->sync();
}


void KViewPart::readDown()
{
  QScrollBar *sb = multiPage->scrollView()->verticalScrollBar();
  if (sb)
    {
      if (sb->value() == sb->maxValue())
	{
	  nextPage();
	  QScrollView *sv = multiPage->scrollView();
	  scrollTo(sv->contentsX(), 0);
	}
      else
	sb->addPage();
    }
}


KViewPartExtension::KViewPartExtension(KViewPart *parent)
  : KParts::BrowserExtension( parent, "KViewPartExtension")
{
}
