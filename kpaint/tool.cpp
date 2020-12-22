// $Id: tool.cpp 107633 2001-07-25 15:35:16Z rich $

#include <kdebug.h>
#include <stdio.h>
#include <assert.h>
#include <qpixmap.h>
#include "tool.h"

Tool::Tool()
  : id( -1 ), active( false ),
    pix( 0 ), canvas( 0 ), 
    props( 0 ), special( false )
{
}

Tool::Tool(const QString & toolname)
  : id( -1 ), active( false ),
  pix( 0 ), name(toolname), canvas( 0 ),
  props( 0 ), special( false )
{
}

void Tool::activate(Canvas *c)
{
  assert(!c->isActive());
  canvas= c;
  active= true;
  activating();
}

void Tool::deactivate()
{
  assert(active);
  deactivating();
  active= false;
  canvas= 0;
}

void Tool::setLeftPen(QPen &p)
{
  kdDebug(4400) << "Tool::setLeftPen()" << endl;
  leftpen= p;
}

void Tool::setLeftBrush(QBrush &b)
{
  kdDebug(4400) << "Tool::setLeftBrush()" << endl;
  leftbrush= b;
}

void Tool::setRightPen(QPen &p)
{
  kdDebug(4400) << "Tool::setRightPen()" << endl;
  rightpen= p;
}

void Tool::setRightBrush(QBrush &b)
{
  kdDebug(4400) << "Tool::setRightBrush()" << endl;
  rightbrush= b;
}

#include "tool.moc"
