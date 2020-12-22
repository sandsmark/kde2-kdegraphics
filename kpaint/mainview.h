// -*- c++ -*-

#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <qwidget.h>

class View;
class QScrollView;
class Canvas;
class QBoxLayout;

/**
 * view window for kpaint just created for layout.
 */
class MainView : public QWidget
{
  Q_OBJECT
public:
  MainView(QWidget *parent=0, const char *name=0);
  ~MainView();

  QScrollView *getViewport() const { return v; }
  Canvas *getCanvas() const { return c; }

public slots:
  void updateLayout();

protected:
  void resizeEvent( QResizeEvent *e );

private:
  // Child widgets
  QScrollView *v;
  Canvas *c;

  // Layoutmanager
  QBoxLayout *lm;
};

#endif // MAINVIEW_H
