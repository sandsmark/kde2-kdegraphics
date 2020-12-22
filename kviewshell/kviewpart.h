#ifndef __KVIEWPART_H
#define __KVIEWPART_H


#include <qlist.h>


#include <kparts/part.h>
#include <kparts/browserextension.h>
#include <kparts/factory.h>
#include <kdirwatch.h>


class KAction;
class KSelectAction;
class KToggleAction;
class ScrollBox;
class MarkList;
class KInstance;
class KViewPartExtension;
class KAccel;
class GotoDialog;
class QSize;
class PaperSize;


#include "kmultipage.h"


class KViewPartFactory : public KParts::Factory
{
  Q_OBJECT

public:

  KViewPartFactory();
  virtual ~KViewPartFactory();

  virtual KParts::Part *createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *classname, const QStringList &args );

  static KInstance *instance();

private:

  static KInstance *s_instance;

};


class KViewPart : public KParts::ReadOnlyPart
{
  Q_OBJECT

public:
  // Define the maximal and minimal zoom factor which is accepted by
  // the kviewshell. If the user or a configuration file asks for
  // smaller or bigger values, the kviewshell uses either minZoom or
  // maxZoom. Implementations of the parts should also use these
  // limits if they whish to implement limits on the allowed zoom
  // factors.
  // Since ISO C++ does not allow floating point constants <sigh>, we
  // use integers which need to be divided by 1000 before use. In this
  // case, this means minZoom = 0.01 and maxZoom = 10.
  static const unsigned int minZoom = 10;
  static const unsigned int maxZoom = 10000;

  KViewPart(QString partname, QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name);
  virtual ~KViewPart();

  void readSettings();
  void writeSettings();


public slots:
  virtual bool closeURL();

  QStringList fileFormats();


protected slots:

  void slotShowMarkList();

  void slotOrientation (int);
  void slotMedia (int);

  void prevPage();
  void nextPage();
  void firstPage();
  void lastPage();
  void goToPage();

  void zoomIn();
  void zoomOut();

  void fitToPage();
  void fitSize();
  void fitToHeight();
  void fitToWidth();

  void scrollUp();
  void scrollDown();
  void scrollLeft();
  void scrollRight();

  void scrollUpPage();
  void scrollDownPage();
  void scrollLeftPage();
  void scrollRightPage();

  void readDown();

  void slotPrint();

  void fileChanged(const QString&);

  void updatePreview(bool previewAvailable=false);

  void slotPreview();


protected:

  KToggleAction *showmarklist, *showPreview;
  KSelectAction *orientation, *media;

  virtual bool openFile();

  int page() { return _currentPage; };
  int pages() { return _numberOfPages; };
  void setPage(int page);

  bool eventFilter(QObject *obj, QEvent *ev);

  QList<PaperSize> _paperSizes;
  void fillPaperSizes();

  void setPaperSize(double w, double h);


private slots:

  void numberOfPages(int nr);
  void pageInfo(int numpages, int currentpage);
  void pageSelected(int nr);
  void contentsMoving(int x, int y);
  void scrollBoxChanged(QPoint np);
  void updateScrollBox();
  void scrollTo(int x, int y);
  void slotGotoDialog(const QString &page);


private:

  KDirWatch watch;

  QString _partname;

  void checkActions();

  QSize pageSize();

  KAccel *accel;

  KAction *zoomInAct, *zoomOutAct, *backAct, *forwardAct,
    *startAct, *endAct, *endDocAct, *markAct, *gotoAct,
    *fitAct, *fitPageAct, *fitHeightAct, *fitWidthAct,
    *saveAction, *printAction, *readDownAct;
  KToggleAction *watchAct;

  KMultiPage *multiPage;

  ScrollBox *scrollBox;
  MarkList  *markList;

  KViewPartExtension *m_extension;

  int _numberOfPages, _currentPage;

  // Current zoom factor of the windows. Must satisfy 
  //
  // minZoom <= _zoom <= maxZoom
  //
  // A zoom factor of 1.0 indicates that the image on the screen
  // should be the same size a printout would have. Provided that
  // XWindows is properly installed and is able to tell us the correct
  // screen resolution...
  double _zoom;

  QWidget *mainWidget;

  GotoDialog *_gotoDialog;

  // Paper size in centimeters
  double _paperWidth, _paperHeight;

  QPoint mousePos;

};


class KViewPartExtension : public KParts::BrowserExtension
{
  Q_OBJECT
  friend class KViewPart;

public:

  KViewPartExtension(KViewPart *parent);
  virtual ~KViewPartExtension() {}

};


#endif


