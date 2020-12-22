#include <config.h>
#include <kaction.h>
#include <kaboutdata.h>
#include <kaboutdialog.h>
#include <kapp.h>
#include <kbugreport.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kimageeffect.h>
#include <kinstance.h>
#include <kmessagebox.h>
#include <kprinter.h>
#include <qobject.h>
#include <qlabel.h>
#include <qstring.h>
#include <qimage.h>
#include <qpixmap.h>

// #include "../kviewshell/centeringScrollView.h"
#include "fontpool.h"
#include "kviewpart.h"
#include "optiondialog.h"
#include "kdvi_multipage.h"

extern "C"
{
  void *init_libkdvi()
  {
    KGlobal::locale()->insertCatalogue("kviewshell");
    return new KDVIMultiPageFactory;
  }
};


KInstance *KDVIMultiPageFactory::s_instance = 0L;


KDVIMultiPageFactory::KDVIMultiPageFactory()
{
}


KDVIMultiPageFactory::~KDVIMultiPageFactory()
{
  if (s_instance)
    delete s_instance;

  s_instance = 0;
}


KParts::Part *KDVIMultiPageFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *, const QStringList & )
{
  KParts::Part *obj = new KDVIMultiPage(parentWidget, widgetName, parent, name);
  emit objectCreated(obj);
  return obj;
}


KInstance *KDVIMultiPageFactory::instance()
{
  if (!s_instance)
    s_instance = new KInstance("kdvi");
  return s_instance;
}


KDVIMultiPage::KDVIMultiPage(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name)
  : KMultiPage(parentWidget, widgetName, parent, name), window(0), options(0)
{
  timer_id = -1;
  setInstance(KDVIMultiPageFactory::instance()); 

  printer = 0;
  window = new dviWindow( 1.0, true, scrollView());
  preferencesChanged();

  connect( window, SIGNAL( setStatusBarText( const QString& ) ), this, SIGNAL( setStatusBarText( const QString& ) ) );
  docInfoAction   = new KAction(i18n("Document &Info"), 0, this, SLOT(doInfo()), actionCollection(), "info_dvi");
  exportPSAction  = new KAction(i18n("PostScript"), 0, this, SLOT(doExportPS()), actionCollection(), "export_postscript");
  exportPDFAction = new KAction(i18n("PDF"), 0, this, SLOT(doExportPDF()), actionCollection(), "export_pdf");

  new KAction(i18n("&DVI Options"), 0, this, SLOT(doSettings()), actionCollection(), "settings_dvi");
  new KAction(i18n("About the KDVI plugin..."), 0, this, SLOT(about()), actionCollection(), "about_kdvi");
  new KAction(i18n("Help on the KDVI plugin..."), 0, this, SLOT(helpme()), actionCollection(), "help_dvi");
  new KAction(i18n("Report Bug in the KDVI plugin..."), 0, this, SLOT(bugform()), actionCollection(), "bug_dvi");

  setXMLFile("kdvi_part.rc");

  //@@@  scrollView()->setPage(window);
  scrollView()->addChild(window);
  connect(window, SIGNAL(request_goto_page(int, int)), this, SLOT(goto_page(int, int) ) );
  connect(window, SIGNAL(contents_changed(void)), this, SLOT(contents_of_dviwin_changed(void)) );

  readSettings();
  enableActions(false);
}


KDVIMultiPage::~KDVIMultiPage()
{
  if (timer_id != -1)
    killTimer(timer_id);
  timer_id = -1;
  writeSettings();
  if (printer != 0)
    delete printer;
}


bool KDVIMultiPage::openFile()
{
  emit setStatusBarText(QString(i18n("Loading file %1")).arg(m_file));

  bool r = window->setFile(m_file);
  window->gotoPage(1);
  window->changePageSize(); //  This also calles drawPage();
  
  emit numberOfPages(window->totalPages());
  enableActions(r);

  return r;
}


void KDVIMultiPage::contents_of_dviwin_changed(void)
{
  emit previewChanged(true);
}


bool KDVIMultiPage::closeURL()
{
  window->setFile(""); // That means: close the file. Resize the widget to 0x0.
  enableActions(false);
  return true;
}


QStringList KDVIMultiPage::fileFormats()
{
  QStringList r;
  r << i18n("*.dvi *.DVI|TeX Device Independent files (*.dvi)");
  return r;
}


bool KDVIMultiPage::gotoPage(int page)
{
  window->gotoPage(page+1);
  return true;
}

void KDVIMultiPage::goto_page(int page, int y)
{
  window->gotoPage(page+1, y);
  scrollView()->ensureVisible(scrollView()->width()/2, y );
  emit pageInfo(window->totalPages(), page );
}


double KDVIMultiPage::setZoom(double zoom)
{
  if (zoom < KViewPart::minZoom/1000.0)
    zoom = KViewPart::minZoom/1000.0;
  if (zoom > KViewPart::maxZoom/1000.0)
    zoom = KViewPart::maxZoom/1000.0;

  double z = window->setZoom(zoom);
  scrollView()->resizeContents(window->width(), window->height());

  return z;
}


double KDVIMultiPage::zoomForHeight(int height)
{
  return (double)(height-1)/(window->xres*(window->paper_height/2.54));
}


double KDVIMultiPage::zoomForWidth(int width)
{
  return (double)(width-1)/(window->xres*(window->paper_width/2.54));
}


void KDVIMultiPage::setPaperSize(double w, double h)
{
  window->setPaper(w, h);
}


bool KDVIMultiPage::preview(QPainter *p, int w, int h)
{
  QPixmap *map = window->pix();

  if (!map)
    return false;

  p->scale((double)w/(double)map->width(), (double)h/(double)map->height());
  p->drawPixmap(0, 0, *map);

  return true;
}


void KDVIMultiPage::doInfo(void)
{
  window->showInfo();
}

void KDVIMultiPage::doExportPS(void)
{
  window->exportPS();
}

void KDVIMultiPage::doExportPDF(void)
{
  window->exportPDF();
}

void KDVIMultiPage::doSettings()
{
  if (options) {
    options->show();
    return;
  }

  options = new OptionDialog(window);
  connect(options, SIGNAL(preferencesChanged()), this, SLOT(preferencesChanged()));
  options->show();
}

void KDVIMultiPage::about()
{
  KAboutDialog *ab = new KAboutDialog(KAboutDialog::AbtAppStandard, 
				      i18n("the KDVI plugin"), 
				      KAboutDialog::Close, KAboutDialog::Close);

  ab->setProduct("kdvi", "0.9h", QString::null, QString::null);
  ab->addTextPage (i18n("About"), 
		   i18n("A previewer for Device Independent files (DVI files) produced "
			"by the TeX typesetting system.<br>"
			"Based on kdvi 0.4.3 and on xdvik, version 18f.<br><hr>"
			"For latest information, visit "
			"<a href=\"http://devel-home.kde.org/~kdvi\">KDVI's Homepage</a>."),
		   true);
  ab->addTextPage (i18n("Authors"), 
		   i18n("Stefan Kebekus<br>"
			"<a href=\"http://btm8x5.mat.uni-bayreuth.de/~kebekus\">"
			"http://btm8x5.mat.uni-bayreuth.de/~kebekus</a><br>"
			"<a href=\"mailto:kebekus@kde.org\">kebekus@kde.org</a><br>"
			"Current maintainer of kdvi. Major rewrite of version 0.4.3."
			"Implementation of hyperlinks.<br>"
			"<hr>"
			"Markku Hinhala<br>"
			"Author of kdvi 0.4.3"
			"<hr>"
			"Nicolai Langfeldt<br>"
			"Maintainer of xdvik"
			"<hr>"
			"Paul Vojta<br>"
			" Author of xdvi<br>"
			"<hr>"
			"Many others. Really, lots of people who were involved in kdvi, xdvik and "
			"xdvi. I apologize to those who I did not mention here. Please send me an "
			"email if you think your name belongs here."),
		   true);
  ab->setMinimumWidth(500);
  ab->show();
}

void KDVIMultiPage::bugform()
{
  KAboutData *kab = new KAboutData("kdvi", I18N_NOOP("KDVI"), "0.9h", 0, 0, 0, 0, 0);
  KBugReport *kbr = new KBugReport(0, true, kab );
  kbr->show();
}

void KDVIMultiPage::helpme()
{
  kapp->invokeHelp( "", "kdvi" );
}

void KDVIMultiPage::preferencesChanged()
{
#ifdef DEBUG_MULTIPAGE
  kdDebug(4300) << "preferencesChanged" << endl;
#endif

  KConfig *config = instance()->config();

  QString s;

  config->reparseConfiguration();
  config->setGroup( "kdvi" );

  int mfmode = config->readNumEntry( "MetafontMode", DefaultMFMode );
  if (( mfmode < 0 ) || (mfmode >= NumberOfMFModes))
    config->writeEntry( "MetafontMode", mfmode = DefaultMFMode );
  window->setMetafontMode( mfmode );

  int makepk = config->readBoolEntry( "MakePK", true );
  if ( makepk != window->makePK() )
    window->setMakePK( makepk );

  int showPS = config->readNumEntry( "ShowPS", 1 );
  if (showPS != window->showPS())
    window->setShowPS(showPS);

  int showHyperLinks = config->readNumEntry( "ShowHyperLinks", 1 );
  if (showHyperLinks != window->showHyperLinks())
    window->setShowHyperLinks(showHyperLinks);
}


bool KDVIMultiPage::print(const QStringList &pages, int current)
{
  // Make sure the KPrinter is available
  if (printer == 0) {
    printer = new KPrinter();
    if (printer == 0)
      return false;
  }

  // Feed the printer with useful defaults and information.
  printer->setPageSelection( KPrinter::ApplicationSide );
  printer->setCurrentPage( current+1 ); 
  printer->setMinMax( 1, window->totalPages() );

  // If pages are marked, give a list of marked pages to the
  // printer. We try to be smart and optimize the list by using ranges
  // ("5-11") wherever possible. The user will be tankful for
  // that. Complicated? Yeah, but that's life.
  if (pages.isEmpty() == true)
    printer->setOption( "kde-range", "" );
  else {
    int commaflag = 0;
    QString range;
    QStringList::ConstIterator it = pages.begin();
    do{
      int val = (*it).toUInt()+1;
      if (commaflag == 1) 
	range +=  QString(", ");
      else
	commaflag = 1;
      int endval = val;
      if (it != pages.end()) {
	QStringList::ConstIterator jt = it;
	jt++;
	do{
	  int val2 = (*jt).toUInt()+1;
	  if (val2 == endval+1)
	    endval++;
	  else
	    break;
	  jt++;
	} while( jt != pages.end() );
	it = jt;
      } else
	it++;
      if (endval == val)
	range +=  QString("%1").arg(val);
      else
	range +=  QString("%1-%2").arg(val).arg(endval);
    } while (it != pages.end() );
    printer->setOption( "kde-range", range );
  }

  // Show the printer options requestor
  if (printer->setup(window) == false) 
    return false;
  if (printer->pageList().isEmpty() == true) {
    KMessageBox::error( window,
			i18n("The list of pages you selected was empty.\n"
			     "Maybe you made an error in selecting the pages, \n"
			     "e.g. by giving in invalid range like '7-2'.") );
    return false;
  }

  // Turn the results of the options requestor into a list arguments
  // which are used by dvips.
  QString dvips_options;
  // Number of copies and collated copies.
  int copies = printer->numCopies();
  if (copies > 1)
    if (printer->collate() == KPrinter::Collate)
      dvips_options += QString("-C %1 ").arg(copies);
    else
      dvips_options += QString("-c %1 ").arg(copies);
  // Print in reverse order.
  if ( printer->pageOrder() == KPrinter::LastPageFirst )
    dvips_options += "-r ";
  // Print only odd pages.
  if ( printer->pageSet() == KPrinter::OddPages )
    dvips_options += "-A ";
  // Print only even pages.
  if ( printer->pageSet() == KPrinter::EvenPages )
    dvips_options += "-B ";
  // Orientation
  if ( printer->orientation() == KPrinter::Landscape )
    dvips_options += "-t landscape ";
  // List of pages to print.
  QValueList<int> pageList = printer->pageList();
  dvips_options += "-pp ";
  int commaflag = 0;
  for( QValueList<int>::ConstIterator it = pageList.begin(); it != pageList.end(); ++it ) {
    if (commaflag == 1) 
      dvips_options +=  QString(",");
    else
      commaflag = 1;
    dvips_options += QString("%1").arg(*it);
  }
  kdDebug() << "dvips_options " << dvips_options << "\n";

  // Now print. For that, export the DVI-File to PostScript. Note that
  // dvips will run concurrently to keep the GUI responsive, keep log
  // of dvips and allow abort. Giving a non-zero printer argument
  // means that the dvi-widget will print the file when dvips
  // terminates, and then delete the output file.
  KTempFile tf;
  kdDebug() << "TMP " << tf.name() << "\n";
  window->exportPS(tf.name(), dvips_options, printer);

  // "True" may be a bit euphemistic. However, since dvips runs
  // concurrently, there is no way of telling the result of the
  // printing command now.
  return true;
}


// Explanation of the timerEvent.
//
// This is a dreadful hack. The problem we adress with this timer
// event is the following: the kviewshell has a KDirWatch object which
// looks at the DVI file and calls reload() when the object has
// changed. That works very nicely in principle, but in practise, when
// TeX runs for several seconds over a complicated file, this does not
// work at all. First, most of the time, while TeX is still writing,
// the file is invalid. Thus, reload() is very often called when the
// DVI file is bad. We solve this problem by checking the file
// first. If the file is bad, we do not reload. Second, when the file
// finally becomes good, it very often happens that KDirWatch does not
// notify us anymore. Whether this is a bug or a side effect of a
// feature of KDirWatch, I dare not say. We remedy that problem by
// using a timer: when reload() was called on a bad file, we
// automatically come back (via the timerEvent() function) every
// second and check if the file becaome good. If so, we stop the
// timer. It may well happen that KDirWatch calls us several times
// while we are waiting for the file to become good, but that does not
// do any harm.
//
// -- Stefan Kebekus.

void KDVIMultiPage::timerEvent( QTimerEvent * )
{
#ifdef DEBUG_MULTIPAGE
  kdDebug(4300) << "Timer Event " << endl;
#endif
  reload();
}

void KDVIMultiPage::reload()
{
#ifdef DEBUG_MULTIPAGE
  kdDebug(4300) << "Reload file " << m_file << endl;
#endif

  if (window->correctDVI(m_file)) {
    killTimer(timer_id);
    timer_id = -1;
    int currsav = window->curr_page();

    bool r = window->setFile(m_file);
    enableActions(r);

    // Go to the old page and tell kviewshell where we are.
    window->gotoPage(currsav);
    // We don't use "currsav" here, because that page may no longer
    // exist. In that case, gotoPage already selected another page.
    emit pageInfo(window->totalPages(), window->curr_page()-1 ); 
  } else {
    if (timer_id == -1)
      timer_id = startTimer(1000);
  }
}

void KDVIMultiPage::enableActions(bool b)
{
  docInfoAction->setEnabled(b);
  exportPSAction->setEnabled(b);
  exportPDFAction->setEnabled(b);
}

#include "kdvi_multipage.moc"
