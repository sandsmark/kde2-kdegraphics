#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <qobject.h>
#include <qlabel.h>
#include <qscrollview.h>
#include <qimage.h>
#include <qpixmap.h>
#include <kprinter.h>
#include <qpaintdevicemetrics.h>
#include <qfile.h>

#include <kinstance.h>
#include <klocale.h>
#include <kdebug.h>
#include <kimageeffect.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kimageio.h>
#include <kstddirs.h>


#include <tiffio.h>


#include "kfax_multipage.moc"


extern "C"
{
  void *init_libkfax()
  {
    return new KFaxMultiPageFactory;
  }
};


KInstance *KFaxMultiPageFactory::s_instance = 0L;


KFaxMultiPageFactory::KFaxMultiPageFactory()
{
}


KFaxMultiPageFactory::~KFaxMultiPageFactory()
{
  if (s_instance)
    delete s_instance;

  s_instance = 0;
}

KParts::Part *KFaxMultiPageFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *, const QStringList & )
{
  KParts::Part *obj = new KFaxMultiPage( parentWidget, widgetName, parent, name);
  emit objectCreated(obj);
  return obj;
}


KInstance *KFaxMultiPageFactory::instance()
{
  if (!s_instance)
    {
      s_instance = new KInstance("kfax");
      KImageIO::registerFormats();
    }

  return s_instance;
}


KFaxMultiPage::KFaxMultiPage(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name)
  : KMultiPage(parentWidget, widgetName, parent, name), faxPages(), _currentPage(-1), _zoom(1.0)
{
  setInstance(KFaxMultiPageFactory::instance());

  window = new QWidget(scrollView());
  window->resize(0,0);

  antiAct = new KToggleAction(i18n("&Anti aliasing"), 0, this,
			      SLOT(toggleAnti()), actionCollection(),
			      "settings_anti");

  setXMLFile("kfax_part.rc");

  scrollView()->addChild(window);

  faxPages.setAutoDelete(true);

  readSettings();
}


KFaxMultiPage::~KFaxMultiPage()
{
  writeSettings();
}


bool KFaxMultiPage::openFile()
{
  faxPages.clear();

  // not a TIFF?
  TIFF *tif = TIFFOpen(QFile::encodeName(m_file), "r");
  if (!tif)
    return openFAX(m_file);
  else
    return openTIFF(tif);
}


extern "C" { int tiffcp(TIFF* in, TIFF* out); }


bool KFaxMultiPage::openTIFF(TIFF *tif)
{
  // iterate directory
  int count=1;
  while (TIFFReadDirectory(tif))
    count++;

  // tell how many pages we found
  emit numberOfPages(count);

  if (count > 1)
    {
      TIFFClose(tif);

      tif = TIFFOpen(QFile::encodeName(m_file), "r");
      if (!tif)
	return false;

      do
	{
	  QString fname = KGlobal::dirs()->saveLocation("data", "kfax/faxes");
	  fname += "/faxXXXXXX";
	  QCString buffer = QFile::encodeName(fname);
	  int fd=mkstemp(buffer.data());
	  TIFF *tif2 = TIFFFdOpen(fd, buffer, "w");
	  if (!tif2)
	    {
	      close(fd);
	      TIFFClose(tif);
	      return false;
	    }
	  tiffcp(tif, tif2);
	  TIFFClose(tif2);
	  faxPages.append(new KFaxPage(buffer, "G3"));
	}
      while (TIFFReadDirectory(tif));
    }
  else
    faxPages.append(new KFaxPage(m_file, "G3"));

  TIFFClose(tif);

  gotoPage(0);

  emit previewChanged(true);

  return true;
}


bool KFaxMultiPage::openFAX(QString filename)
{
  return false;
}


bool KFaxMultiPage::closeURL()
{
  window->setBackgroundPixmap(QPixmap());
  scrollView()->resizeContents(0, 0);

  faxPages.clear();
  _currentPage = -1;

  emit previewChanged(false);

  return true;
}


QStringList KFaxMultiPage::fileFormats()
{
  QStringList r;
  r << i18n("*.g3|Fax files (*.g3)");
  return r;
}


bool KFaxMultiPage::gotoPage(int page)
{
  if (_currentPage == page)
    return true;

  KFaxPage *fp = faxPages.at(page);
  if (!fp)
    return false;

  int w = (int)_zoom*672.0;
  int h = (int)_zoom*825.0;
  fp->scale(w, h, antiAct->isChecked());
  window->setBackgroundPixmap(fp->scaledPixmap());
  window->resize(w, h);
  scrollView()->resizeContents(w, h);

  _currentPage = page;

  emit previewChanged(true);

  return true;
}


double KFaxMultiPage::setZoom(double zoom)
{
  KFaxPage *fp = faxPages.at(_currentPage);
  if (!fp)
    return 0;
  fp->scale((int)672.0*zoom, (int)825.0*zoom, antiAct->isChecked());
  window->resize((int)672.0*zoom, (int)825.0*zoom);
  window->setBackgroundPixmap(fp->scaledPixmap());
  scrollView()->resizeContents(window->width(), window->height());
  _zoom = zoom;

  return _zoom;
}


double KFaxMultiPage::zoomForHeight(int height)
{
  return (double)height/825.0;
}


double KFaxMultiPage::zoomForWidth(int width)
{
  return (double)width/672.0;
}


void KFaxMultiPage::setPaperSize(double w, double h)
{
  //  window->setPaper(w, h);
}


bool KFaxMultiPage::preview(QPainter *p, int w, int h)
{
  KFaxPage *fp = faxPages.at(_currentPage);
  if (!fp)
    return false;

  fp->preview(p, w, h);
  return true;
}


void KFaxMultiPage::toggleAnti()
{
  KFaxPage *fp = faxPages.at(_currentPage);
  if (!fp)
    return;

  int w = (int)(_zoom*672.0);
  int h = (int)(_zoom*825.0);
  fp->scale(w, h, antiAct->isChecked());
  window->setBackgroundPixmap(fp->scaledPixmap());

  emit previewChanged(true);
}


bool KFaxMultiPage::print(const QStringList &pages, int current)
{
  KPrinter printer;

  printer.setColorMode(KPrinter::GrayScale);
  printer.setFullPage(true);

  if (printer.setup())
    {
      QStringList pagesTmp = pages;
      QStringList::Iterator it = pagesTmp.begin();
      if (printer.pageOrder() == KPrinter::FirstPageFirst)
	{
	  for (; it != pagesTmp.end(); ++it)
	    {
	      KFaxPage *fp = faxPages.at((*it).toInt());
	
	      if (fp)
		{
		  fp->print(&printer);
		  printer.newPage();
		}
	    }
	}
      else
	{
	  for ( it = pagesTmp.fromLast(); it != pagesTmp.end(); --it)
	    {
	      KFaxPage *fp = faxPages.at((*it).toInt());
	
	      if (fp)
		{
		  fp->print(&printer);
		  printer.newPage();
		}
	    }
	}
    }
  return true;
}


void KFaxMultiPage::readSettings()
{
  KConfig *config = instance()->config();

  config->setGroup("GUI");

  antiAct->setChecked(config->readBoolEntry("AntiAliasing", true));
}


void KFaxMultiPage::writeSettings()
{
  KConfig *config = instance()->config();

  config->setGroup("GUI");

  config->writeEntry("AntiAliasing", antiAct->isChecked());

  config->sync();
}


KFaxPage::KFaxPage(QString fname, QString fmt)
  : filename(fname), format(fmt), scalePixmap(), rawImage(),
  antiAliasing(false), preAnti(false), loaded(false)
{
}


void KFaxPage::scale(int w, int h, bool anti)
{
  if (!scalePixmap.isNull()
      && (w == scalePixmap.width()) && (h == scalePixmap.height())
      && (anti == antiAliasing))
    return;

  if (anti != antiAliasing)
    {
      antiAliasing = anti;
      previewPixmap = QPixmap();
    }

  load();

  scalePixmap.resize(w, h);
  QPainter p(&scalePixmap);

  if (antiAliasing)
    p.drawImage(0, 0, rawImage.convertDepth(8).smoothScale(w,h));
  else
    {
      p.scale((double)w/(double)rawImage.width(),
	      (double)h/(double)rawImage.height());
      p.drawImage(0, 0, rawImage);
    }
}


void KFaxPage::preview(QPainter *p, int w, int h)
{
  load();

  if (w != previewPixmap.width() || h != previewPixmap.height())
    {
      previewPixmap.resize(w, h);
      QPainter pp(&previewPixmap);

      pp.scale((double)w/(double)scalePixmap.width(),
	       (double)h/(double)scalePixmap.height());
      pp.drawPixmap(0, 0, scalePixmap);
    }

  p->drawPixmap(0, 0, previewPixmap);
}


void KFaxPage::load()
{
  if (!loaded)
    {
      rawImage.load(filename, format.ascii());
      scalePixmap = QPixmap();
      loaded = true;
    }
}


QPixmap KFaxPage::scaledPixmap()
{
  return scalePixmap;
}


void KFaxPage::print(KPrinter *printer)
{
  QPainter p;
  QPaintDeviceMetrics metric(printer);

  p.begin(printer);

  p.scale((double)metric.width()/(double)rawImage.width(),
	  (double)metric.height()/(double)rawImage.height());
  p.drawImage(0, 0, rawImage);
}


