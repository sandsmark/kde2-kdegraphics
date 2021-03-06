// -*- c++ -*-
// $Id: manager.h 75967 2000-12-31 13:23:18Z jcaliff $

#ifndef MANAGER_H
#define MANAGER_H

#include <qlist.h>
#include <qobject.h>
#include <qpen.h>
#include <qbrush.h>
#include <qwidget.h>
#include "ktoolbar.h"
#include "canvas.h"
#include "properties.h"
#include "tool.h"

typedef QList<Tool> ToolList;
class KStatusBar;
class KColorButton;

/**
 * @short Manager Class
 * This class holds all the tools that can operate on a canvas, it
 * ensures that one tool is always active, and responds to signals
 * from the tool palette which control which tool is selected. A
 * tool manager is associated with a single canvas.
 */
class Manager : public QObject
{
  Q_OBJECT
public:
  Manager(Canvas *c);
  ~Manager();

  inline int getCurrentToolID();
  inline const Tool &getCurrentTool();
  inline const ToolList &getToolList();
  void showPropertiesDialog();
  void populateToolbar(KToolBar *);
  inline void setStatusBar(KStatusBar *);
  inline const QColor &lmbColour();
  inline const QColor &rmbColour();
  void setLMColorButton(KColorButton *cb);
  void setRMColorButton(KColorButton *cb);
   
public slots:
  void setCurrentTool(int);
  void updateProperties();
  void setLMBcolour(const QColor &);
  void setRMBcolour(const QColor &);

signals:
  void toolChanged(int);
  void modified();

protected:
  void createTools();

private:
  ToolList list;
  Canvas *canvas;
  propertiesDialog *props;
  int currentToolID;

  QPen p;
  QBrush b;
  QColor lmbCol;
  QColor rmbCol;
  KToolBar *toolsToolBar;
  KStatusBar *statusBar;
  KColorButton * lmColorButton;
  KColorButton * rmColorButton;
};


const QColor &
Manager::lmbColour()
{
  return lmbCol;
}

const QColor &
Manager::rmbColour()
{
  return rmbCol;
}

int 
Manager::getCurrentToolID()
{
  return currentToolID;
}

const Tool & 
Manager::getCurrentTool()
{
  return *list.at(currentToolID);
}

const ToolList & 
Manager::getToolList()
{
  return list;
}

void 
Manager::setStatusBar(KStatusBar *sb)
{
  statusBar = sb;
}

#endif // MANAGER_H
