// -*- c++ -*-
// $Id: circle.h 59876 2000-08-06 10:25:11Z bieker $

#ifndef CIRCLE_H
#define CIRCLE_H

#include "tool.h"

class Circle : public Tool
{
  Q_OBJECT
public:
  Circle();
  Circle(const QString & toolname);
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
  bool drawing;
  int activeButton;
};

#endif // CIRCLE_H

