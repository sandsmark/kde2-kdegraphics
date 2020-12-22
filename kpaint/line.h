// -*- c++ -*-

// Line class
// This class is a subclass of Tool and provides a rubber line drawing tool.

// $Id: line.h 59876 2000-08-06 10:25:11Z bieker $

#ifndef LINE_H
#define LINE_H

#include <qpixmap.h>
#include "tool.h"

class Line : public Tool
{
  Q_OBJECT
public:
  Line(const QString & toolname);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void activating();
  virtual QPixmap pixmap() const;
signals:
  void modified();
private:
  int startx, starty;
  int lastx, lasty;
  bool drawing;
  int activeButton;
};

#endif // LINE_H

