// -*- c++ -*-

// $Id: canvas.h 107633 2001-07-25 15:35:16Z rich $

#ifndef CANVAS_H
#define CANVAS_H

#include <qwidget.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qwmatrix.h>

class Tool;

class Canvas : public QWidget
{
  Q_OBJECT

public:
  Canvas(int width, int height, QWidget *parent= 0, const char *name=0);
  Canvas(const QString & filename, QWidget *parent= 0, const char *name=0);
  ~Canvas();

  QPixmap *pixmap();
  void setPixmap(QPixmap *);
  void setDepth(int);
  int getDepth() const { return pix->depth(); }

  const QRect &selection() const;
  void setSelection(const QRect&);
  bool hasSelection() const { return haveSelection_; }
  void clearSelection();

  /** copies from selection into clipboard and then cuts selection */
  void cut();
  /** copys from selection into clipboard */
  void copy();
  /* paste from clipboard into canvas */
  void paste( int x, int y );

  //static inline bool hasClipboardData();
  //static inline const QPixmap *clipboardData();

  QPixmap *zoomedPixmap();
  void setZoom(int);
  int zoom();
  void updateZoomed();
  void updateZoomedClipboard(); //jwc
  void resizeImage(int, int);

  bool load(const QString & filename = QString::null, const char *format= 0);
  bool save(const QString & filename = QString::null, const char *format= 0);

  bool isActive();
  bool isModified();
  void clearModified();

  // This controls which tool the events go to (if any)
  void activate(Tool *tool);
  void deactivate();

  // Handle paint events
  void paintEvent(QPaintEvent *e);

  // Redirect events tools can use
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);

public slots:
  void markModified();

signals:
  void sizeChanged();
  void pixmapChanged(QPixmap *);
  void modified();
  void selection(bool);
  void clipboard(bool);

protected:
  enum state {
    ACTIVE,
    INACTIVE
  };

  QWMatrix *matrix;
  Tool *currentTool;

  /** Zoomed copy */
  QPixmap *zoomed;

  /** Master copy */
  QPixmap *pix;

  /** Clipboard Data from Cut/Copy */
  //static QPixmap *clipboardPix;
  QPixmap clipboardPix;

  /** % of original size */
  int zoomFactor;

  /** Has the pixmap been modified? */
  bool modified_;
  state s;
  QRect selection_;
  QRect clipboardRect_; // jwc
  bool haveSelection_;

  static int inst; // how many instances have we ?
  
};

#endif // CANVAS_H
