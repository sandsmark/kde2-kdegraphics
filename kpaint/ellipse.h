// -*- c++ -*-
// $Id: ellipse.h 59876 2000-08-06 10:25:11Z bieker $

#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <qpixmap.h>
#include <qpainter.h>
#include "tool.h"

class Ellipse : public Tool
{
  Q_OBJECT
public:
  Ellipse();
  Ellipse(const QString & toolname);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void activating();
  QPixmap pixmap() const;
signals:
  void modified();
private:
  int startx, starty;
  int lastx, lasty;
  int activeButton;
  bool drawing;
};

#endif // ELLIPSE_H

