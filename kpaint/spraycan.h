// -*- c++ -*-

// $Id: spraycan.h 75967 2000-12-31 13:23:18Z jcaliff $

#ifndef SPRAYCAN_H
#define SPRAYCAN_H

#include <qpixmap.h>
#include <qpainter.h>
#include <qtimer.h>
#include "tool.h"

class SprayCan : public Tool
{
  Q_OBJECT

public:
    SprayCan(const QString & toolname);
    ~SprayCan();
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void activating();
    QPixmap pixmap() const;

signals:
    void modified();

private:
    int density; // Dots per second (approx)
    QTimer *timer;
    int x, y;
    int brushsize;
    bool drawing;
    int activeButton;

private slots:
    void drawDot();
};

#endif // SPRAYCAN_H
