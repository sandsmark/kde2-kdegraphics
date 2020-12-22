// -*- c++ -*-
// $Id: pen.h 59876 2000-08-06 10:25:11Z bieker $

#ifndef PEN_H
#define PEN_H

#include <qpixmap.h>
#include <qpainter.h>
#include "tool.h"

class Pen : public Tool
{
  Q_OBJECT
public:
  Pen(const QString & toolname);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void activating();
    virtual QPixmap pixmap() const;
signals:
  void modified();
private:
  int lastx;
  int lasty;
  bool drawing;
  int activeButton;
};

#endif // PEN_H

