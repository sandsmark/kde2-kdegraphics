// -*- c++ -*-

#ifndef AREASELECT_H
#define AREASELECT_H

#include <qpixmap.h>
#include "tool.h"

class AreaSelect : public Tool
{
Q_OBJECT
public:
AreaSelect(const QString & toolname);
  /*  virtual ~AreaSelect(); */
  virtual void mousePressEvent(QMouseEvent *e);
  virtual void mouseMoveEvent(QMouseEvent *e);
  virtual void mouseReleaseEvent(QMouseEvent *e);

public slots:
  void selection(bool);
  void disableSelection();

protected:
  virtual void activating();
  virtual void deactivating();
  virtual void timerEvent(QTimerEvent *);
  virtual QPixmap pixmap() const;

private:
  void drawFrame();

  int startx, starty;
  int lastx, lasty;
  bool drawing, showedSF;
};

#endif // AREASELECT_H

