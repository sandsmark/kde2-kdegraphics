#ifndef __KIF_DNDLISTBOX_H
#define __KIF_DNDLISTBOX_H

#include <qlistbox.h>

class KIFDndListBox : public QListBox
{
    Q_OBJECT
public:
    KIFDndListBox(QWidget *parent=0, const char *name=0)
        : QListBox(parent, name){
            setAcceptDrops(true); setSelectionMode(QListBox::Extended);
        }
signals:
    void urlDropped(const QString &urlStr);
    void dropFinished();
protected:
    void dragEnterEvent(QDragEnterEvent *ev);
    void dropEvent(QDropEvent *ev);
};

#endif

