// $Id: kundo.cpp 72077 2000-11-25 10:06:51Z mlaurent $

#include <kdebug.h>
#include <stdio.h>
#include "kundo.h"

KUndo::KUndo(int maxsize_= 100)
{
  maxsize= maxsize_;
  infront= 0;
  behind= 0;
  undolist.setAutoDelete(TRUE);
}

KUndo::~KUndo()
{

}

void KUndo::record(KUndoData *d)
{
  KUndoData *t;

  for (t= undolist.next(); t != NULL; t= undolist.next())
    undolist.remove();

  undolist.append(d);
  behind++;
  checksize();
}

KUndoData *KUndo::undo()
{
  assert(canUndo());
  behind--;
  infront++;
  return undolist.prev();
}

KUndoData *KUndo::redo()
{
  assert(canRedo());
  behind++;
  infront--;
  return undolist.next();
}

void KUndo::clear()
{
  while (undolist.removeFirst());
}

void KUndo::checksize()
{
kdDebug(4400) << "KUndo::checksize()\n" << endl;

  if (infront+behind > maxsize) {
    if (behind > 0) {
      undolist.removeFirst();
      behind--;
    }
    else if (infront > 0) {
      undolist.removeLast();
      infront--;
    }
  }
}

