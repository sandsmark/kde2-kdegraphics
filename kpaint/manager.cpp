// $Id: manager.cpp 107633 2001-07-25 15:35:16Z rich $

#include <kdebug.h>
#include <qobject.h>
#include <stdio.h>
#include <assert.h>
#include <qlist.h>
#include <qpixmap.h>
#include <klocale.h>
#include <kstatusbar.h>
#include <kcolorbtn.h>

#include "manager.h"
#include "commands.h"

#include "pen.h"
#include "line.h"
#include "ellipse.h"
#include "circle.h"
#include "rectangle.h"
#include "roundangle.h"
#include "spraycan.h"
#include "areaselect.h"
#include "brush.h"
#include "text.h"

Manager::Manager(Canvas *c)
 : QObject(), toolsToolBar(0L), statusBar(0L)
{
    assert(c != 0);
    assert(!(c->isActive()));

    kdDebug(4400) << "Manager:: Constructing...\n" << endl;

    canvas= c;
    createTools();
    currentToolID= 0;
    list.first()->activate(c);

    lmbCol= red;
    rmbCol= green;

    // Initialise the pens and brushes
    p.setColor(lmbCol);
    b.setColor(lmbCol);
    list.first()->setLeftPen(p);
    list.first()->setLeftBrush(b);
    p.setColor(rmbCol);
    b.setColor(rmbCol);
    list.first()->setRightPen(p);
    list.first()->setRightBrush(b);

    c->activate(list.first());
    connect(this, SIGNAL(modified()), c, SLOT(markModified()));
}

Manager::~Manager()
{
}

void Manager::createTools()
{
  AreaSelect *as; // handled different because of slot
  PasteTool  *pt;
  
  // Create the Tools
  list.setAutoDelete(TRUE);
  list.append(new Ellipse( i18n("Ellipse")));
  list.last()->id = 0;
  list.append(new Circle( i18n("Circle")));
  list.last()->id = 1;
  list.append(new Pen( i18n("Pen")));
  list.last()->id = 2;
  list.append(new Line( i18n("Line")));
  list.last()->id = 3;
  list.append(new Rectangle( i18n("Rectangle")));
  list.last()->id = 4;
  list.append(new Roundangle( i18n("Round Angle")));
  list.last()->id = 5;
  list.append(new SprayCan( i18n("Spray Can")));
  list.last()->id = 6;
  list.append(new Text( i18n("Text")));
  list.last()->id = 7;
  
  as = new AreaSelect( i18n("Area Select"));
  connect(canvas, SIGNAL(selection(bool)),
	  as, SLOT(selection(bool)));
  list.append(as);
  as->id = 8;

  pt = new PasteTool( i18n("Paste"));
  list.append(pt);
  as->id = 9;

  // Create the properties dialog
  props= new propertiesDialog(1, 0, "tool_properties");
  connect(props, SIGNAL(applyButtonPressed()),
	  this, SLOT(updateProperties()) );
}

void Manager::updateProperties()
{
  p= props->getPen();
  b= props->getBrush();

  p.setColor(lmbCol);
  b.setColor(lmbCol);
  list.at(currentToolID)->setLeftPen(p);
  list.at(currentToolID)->setLeftBrush(b);
  p.setColor(rmbCol);
  b.setColor(rmbCol);
  list.at(currentToolID)->setRightPen(p);
  list.at(currentToolID)->setRightBrush(b);
}

void Manager::setLMColorButton(KColorButton *cb)
{
    lmColorButton = cb;
}

void Manager::setRMColorButton(KColorButton *cb)
{
    rmColorButton = cb;
}

void Manager::setLMBcolour(const QColor &c)
{
  kdWarning() << "setLMBcolour" << endl;
  lmbCol= c;
  lmColorButton->setColor(c);
  updateProperties();
}

void Manager::setRMBcolour(const QColor &c)
{
  kdWarning() << "setRMBcolour" << endl;
  rmbCol= c;
  updateProperties();
}


void Manager::setCurrentTool(int tool)
{

    /*
    kdDebug(4400) << "got setCurrentTool " << tool << "\n" << endl;
    kdDebug(4400) << " old Tool is " << currentToolID << "\n" << endl;
    */
    
    Tool *t;

    if (currentToolID != tool) 
    {
        canvas->deactivate();
        list.at(currentToolID)->deactivate();
        currentToolID= tool;
        t= list.at(tool);
        t->activate(canvas);

        p.setColor(lmbCol);
        b.setColor(lmbCol);
        t->setLeftPen(p);
        t->setLeftBrush(b);
        p.setColor(rmbCol);
        b.setColor(rmbCol);
        t->setRightPen(p);
        t->setRightBrush(b);

	canvas->updateZoomed();
        canvas->activate(t);
        props->setPages(t->getPages());
        props->repaint(0);

        if (NULL != statusBar) 
        {
            statusBar->changeItem(t->getName(), ID_TOOL_NAME);
        }
        emit toolChanged(tool);
    }
}


void Manager::showPropertiesDialog()
{
    props->show();
}

void Manager::populateToolbar(KToolBar *t)
{
    Tool *tool;
    int i;
    QString tip;

    toolsToolBar = t;

    for(i= 0, tool= list.first(); tool != 0; i++, tool= list.next()) 
    {
        tip= tool->tip();
        t->insertButton(tool->pixmap(), i, TRUE, tip, i);
        tool->id= i;
    }
    t->setButton(currentToolID, true);
    connect(t, SIGNAL(clicked(int)), SLOT(setCurrentTool(int)));
}

#include "manager.moc"

