// -*- c++ -*-

#ifndef BRUSH_H
#define BRUSH_H

#include <qpixmap.h>
#include "tool.h"


class PasteTool : public Tool
{
  Q_OBJECT
public:
  PasteTool(const QString & toolname);
  ~PasteTool() {}

  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

protected:
  void activating();
  void deactivating();

  QPixmap pixmap() const;

private:
  int lastx, lasty;
  bool drawing, showedSF;
  bool noContent;
  QPixmap cPixmap;
};

#endif // BRUSH_H

