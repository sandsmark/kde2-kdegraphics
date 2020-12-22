#ifndef __KPAGETEST_H
#define __KPAGETEST_H


#include <qstringlist.h>


#include <kparts/part.h>
#include <kparts/browserextension.h>
#include <kparts/factory.h>


class QLabel;


#include "kmultipage.h"


class KPageTestFactory : public KParts::Factory
{
  Q_OBJECT

public:

  KPageTestFactory();
  virtual ~KPageTestFactory();

  virtual KParts::Part *createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *, const QStringList & );

  static KInstance *instance();

private:

  static KInstance *s_instance;

};


class KPageTest : public KMultiPage
{
  Q_OBJECT

public:

  KPageTest(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name);
  virtual ~KPageTest();


// Interface definition start ------------------------------------------------

  /// returns the list of supported file formats
  virtual QStringList fileFormats();

  /// opens a file
  virtual bool openFile();

  virtual bool closeURL();

  /// displays the given page
  virtual bool gotoPage(int page);

  /// sets a zoom factor
  virtual double setZoom(double z);

  /// calculates the zoom needed to fit into a given width
  virtual double zoomForWidth(int width);

  /// calculates the zoom needed to fit into a given height
  virtual double zoomForHeight(int height);

  virtual void setPaperSize(double, double) {}

  virtual bool print(const QStringList &pages, int current) { return false; }

signals:

  /// emitted to indicate the number of pages in the file
  void numberOfPages(int nr);

private:

  QLabel *mainWidget;

};


#endif


