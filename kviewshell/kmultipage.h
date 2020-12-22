#ifndef __KMULTIPAGE_H
#define __KMULTIPAGE_H


#include "centeringScrollview.h"

#include <klibloader.h>
#include <kparts/part.h>
#include <kparts/browserextension.h>
#include <qstringlist.h>

class KInstance;
class KMultiPageExtension;
class QLabel;
class QScrollView;
class QPainter;


class KMultiPage : public KParts::ReadOnlyPart
{
  Q_OBJECT

public:

  KMultiPage(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name);
  virtual ~KMultiPage();

  /// returns the scrollview used for the display
  virtual CenteringScrollview *scrollView() { return _scrollView; };


// Interface definition start ------------------------------------------------

  /// returns the list of supported file formats
  virtual QStringList fileFormats() = 0;

  /// opens a file
  virtual bool openFile() = 0;

  /// closes a file
  virtual bool closeURL() = 0;

  /// displays the given page
  virtual bool gotoPage(int page) = 0;

  /// sets a zoom factor. The multipage implementation might refuse to
  //use a given zoom factor, even if it falls within the bounds given
  //by the static constants minZoom and maxZoom which are defined in
  //kviewpart.h. In that case, the multipage implementation chooses a
  //different zomm factor. The implementation returns the factor which
  //has actually been used.
  virtual double setZoom(double z) = 0;

  /// calculates the zoom needed to fit into a given width
  virtual double zoomForWidth(int width) = 0;

  /// calculates the zoom needed to fit into a given height
  virtual double zoomForHeight(int height) = 0;

  /// sets the paper size
  virtual void setPaperSize(double w, double h) = 0;

  /// returns a nice little preview of the page
  virtual bool preview(QPainter *, int, int) { return false; };

  /// prints
  virtual bool print(const QStringList &pages, int current) = 0;

  /// reads in settings
  virtual void readSettings() {};

  /// writes settings
  virtual void writeSettings() {};

  /// reloads the file form disk and redisplays
  virtual void reload() {};

  /// redisplays the contents of the current page
  virtual void redisplay() {} ;

public slots:
  /** Opens a file requestor and starts a basic copy KIO-Job. A
      multipage implementation that wishes to offer saving in various
      formats must re-implement this slot. */
  void slotSave();

  /** Used  internally. */
  void slotIOJobFinished ( KIO::Job *job );

signals:

  /// emitted to indicate the number of pages in the file
  void numberOfPages(int nr);

  /// emitted when a new preview is available
  void previewChanged(bool previewAvailable);


// Interface definition end --------------------------------------------------


private:

  QLabel *mainWidget;

  CenteringScrollview *_scrollView;
};


#endif


