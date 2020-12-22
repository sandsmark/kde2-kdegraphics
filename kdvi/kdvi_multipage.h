#ifndef __KDVIMULTIPAGE_H
#define __KDVIMULTIPAGE_H


#include <qstringlist.h>


#include <kparts/part.h>
#include <kparts/browserextension.h>
#include <kparts/factory.h>

class KPrinter;
class OptionDialog;
class QLabel;
class QPainter;


#include "../kviewshell/kmultipage.h"
#include "dviwin.h"


class KDVIMultiPageFactory : public KParts::Factory
{
  Q_OBJECT

public:

  KDVIMultiPageFactory();
  virtual ~KDVIMultiPageFactory();

  virtual KParts::Part *createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *, const QStringList & );

  static KInstance *instance();

private:

  static KInstance *s_instance;
};


class KDVIMultiPage : public KMultiPage
{
  Q_OBJECT

public:

  KDVIMultiPage(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name);
  virtual ~KDVIMultiPage();


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

protected:
  /// For internal use only. See the comments in kdvi_multipage.cpp, right
  //before the timerEvent function.
  int  timer_id;
  /// For internal use only. See the comments in kdvi_multipage.cpp, right
  //before the timerEvent function.
  void timerEvent( QTimerEvent *e );

  virtual void reload();


signals:

  /** Emitted to indicate the number of pages in the file. The
      receiver will set the current page to zero and call the
      gotoPage()-method. */
  void numberOfPages(int nr);

  /** Emitted to indicate the number of pages in the file and the
      current page. The receiver will not change or update the
      display, nor call the gotoPage()-method. */
  void pageInfo(int nr, int currpg);

protected slots:
  void doSettings();
  void doInfo();
  void doExportPS();
  void doExportPDF();
  void about();
  void helpme();
  void bugform();
  void preferencesChanged();
  void goto_page(int page, int y);
  void contents_of_dviwin_changed(void);

private:

  dviWindow    *window;
  OptionDialog *options;
  KPrinter     *printer;

  /** Pointers to several actions which are disabled if no file is
      loaded. */
  KAction      *docInfoAction;
  KAction      *exportPSAction;
  KAction      *exportPDFAction;

  /** Used to enable the export menu when a file is successfully
      loaded. */
  void enableActions(bool);
};


#endif


