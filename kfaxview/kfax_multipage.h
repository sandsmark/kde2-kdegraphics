#ifndef __KFaxMULTIPAGE_H
#define __KFaxMULTIPAGE_H


#include <qlist.h>
#include <qpixmap.h>

#include <kaction.h>
#include <kparts/part.h>
#include <kparts/browserextension.h>
#include <kparts/factory.h>


class QLabel;
class QPainter;
class KToggleAction;
class KPrinter;


#include <kmultipage.h>


class KFaxPage
{
public:

  KFaxPage(QString fname, QString fmt);
  ~KFaxPage() {};

  void load();

  void preview(QPainter *p, int w, int h);
  void scale(int w, int h, bool anti);
  QPixmap scaledPixmap();

  void print(KPrinter *p);


private:

  QString filename, format;
  QPixmap scalePixmap, previewPixmap;
  QImage  rawImage;
  bool antiAliasing, preAnti;
  bool loaded;

};


class KFaxMultiPageFactory : public KParts::Factory
{
  Q_OBJECT

public:

  KFaxMultiPageFactory();
  virtual ~KFaxMultiPageFactory();

  KParts::Part *createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *, const QStringList & );

  static KInstance *instance();

private:

  static KInstance *s_instance;

};



class KFaxMultiPage : public KMultiPage
{
  Q_OBJECT

public:

  KFaxMultiPage(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name);
  virtual ~KFaxMultiPage();


// Interface definition start ------------------------------------------------

  /// returns the list of supported file formats
  virtual QStringList fileFormats();

  /// opens a file
  virtual bool openFile();

  /// close a file
  virtual bool closeURL();

  /// displays the given page
  virtual bool gotoPage(int page);

  /// sets a zoom factor
  virtual double setZoom(double z);

  /// calculates the zoom needed to fit into a given width
  virtual double zoomForWidth(int width);

  /// calculates the zoom needed to fit into a given height
  virtual double zoomForHeight(int height);

  virtual void setPaperSize(double, double);

  virtual bool preview(QPainter *p, int w, int h);

  virtual bool print(const QStringList &pages, int current);

  virtual void writeSettings();

  virtual void readSettings();


signals:

  /// emitted to indicate the number of pages in the file
  void numberOfPages(int nr);


protected slots:

  void toggleAnti();


private:

  bool openTIFF(TIFF *tif);
  bool openFAX(QString filename);

  QWidget *window;
  QList<KFaxPage> faxPages;
  KToggleAction *antiAct;
  int _currentPage;

  double _zoom;

};


#endif


