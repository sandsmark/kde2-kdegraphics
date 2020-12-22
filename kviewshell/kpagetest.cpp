#include <qobject.h>
#include <qlabel.h>
#include <qscrollview.h>


#include <kinstance.h>
#include <klocale.h>
#include <kdebug.h>


#include "kpagetest.moc"


extern "C"
{
  void *init_libkpagetest()
  {
    return new KPageTestFactory;
  }
};


KInstance *KPageTestFactory::s_instance = 0L;


KPageTestFactory::KPageTestFactory()
{
}


KPageTestFactory::~KPageTestFactory()
{
  if (s_instance)
    delete s_instance;

  s_instance = 0;
}


KParts::Part *KPageTestFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *, const QStringList & )
{
  KParts::Part *obj = new KPageTest( parentWidget, widgetName, parent, name);
  emit objectCreated(obj);
  return obj;
}


KInstance *KPageTestFactory::instance()
{
  if (!s_instance)
    s_instance = new KInstance("kpagetest");
  return s_instance;
}


KPageTest::KPageTest(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name)
  : KMultiPage(parentWidget, widgetName, parent, name)
{
  setInstance(KPageTestFactory::instance());

  mainWidget = new QLabel(scrollView());
  mainWidget->setAlignment(AlignCenter);
  mainWidget->setText("Test");
  mainWidget->setBackgroundColor(Qt::red);
  mainWidget->resize(300,200);
  mainWidget->setFocusPolicy(QWidget::StrongFocus);

  scrollView()->addChild(mainWidget);
}


KPageTest::~KPageTest()
{
}


bool KPageTest::openFile()
{
  mainWidget->setText(m_file);

  emit numberOfPages(10);

  return true;
}


bool KPageTest::closeURL()
{
  mainWidget->setText("DVI");

  return true;
}


// test code
QStringList KPageTest::fileFormats()
{
  QStringList r;
  r << "*.dvi|DVI files (*dvi)";
  return r;
}


bool KPageTest::gotoPage(int page)
{
  kdDebug() << "New Page number: " << page << endl;
  return true;
}


double KPageTest::setZoom(double zoom)
{
  kdDebug() << "New Zoom Factor: " << zoom << endl;

  mainWidget->resize(300*zoom, 200*zoom);
  scrollView()->resizeContents(300*zoom, 200*zoom);

  return zoom;
}


double KPageTest::zoomForHeight(int height)
{
  return height/200.0;
}


double KPageTest::zoomForWidth(int width)
{
  return width/300.0;
}
