#include "hotlistbox.h"
#include <qdragobject.h>
#include <qpopupmenu.h>
#include <klocale.h>
#include <qtoolbutton.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kstddirs.h>

KIFHotListBox::KIFHotListBox(QWidget *parent, const char *name)
    : QListBox(parent, name)
{
    setAcceptDrops(true);
    addBtn = new QToolButton(this);
    addBtn->setIconSet(QPixmap(KGlobal::dirs()->findResource("appdata",
                                                             "inc2.png")));
    addBtn->setTextLabel(i18n("Add Current Directory"));
    addBtn->resize(addBtn->sizeHint());
    connect(addBtn, SIGNAL(clicked()), this, SLOT(slotAddClicked()));
    
    delBtn = new QToolButton(this);
    delBtn->setIconSet(QPixmap(KGlobal::dirs()->findResource("appdata",
                                                             "dec2.png")));
    delBtn->setTextLabel(i18n("Remove Selected HotList Item"));
    delBtn->resize(delBtn->sizeHint());
    connect(delBtn, SIGNAL(clicked()), this, SLOT(slotDelClicked()));
    setSelectionMode(QListBox::Single);
}

void KIFHotListBox::dragEnterEvent(QDragEnterEvent *ev)
{
    lastDndHighlight = NULL;;
    ev->accept(QUriDrag::canDecode(ev));
}

void KIFHotListBox::dragLeaveEvent(QDragLeaveEvent *)
{
    if(lastDndHighlight)
        setSelected(lastDndHighlight, false);
    lastDndHighlight = NULL;;
}

void KIFHotListBox::dragMoveEvent(QDragMoveEvent *ev)
{
    if(itemAt(ev->pos())){
        QRect r = itemRect(itemAt(ev->pos()));
        // itemAt is supposed to return NULL if not on an item, but returns
        // the last item no matter how far down you are :P
        if(r.contains(ev->pos())){
            if(lastDndHighlight != itemAt(ev->pos())){
                if(currentItem() != -1)
                    setSelected(currentItem(), false);
                lastDndHighlight = itemAt(ev->pos());
                setSelected(lastDndHighlight, true);
            }
            ev->accept();
        }
        else
            ev->ignore();
    }
    else
        ev->ignore();
}


void KIFHotListBox::dropEvent(QDropEvent *ev)
{
    int i = -1;
    if(lastDndHighlight){
        i = currentItem();
        setSelected(lastDndHighlight, false);
    }
    if(i == -1)
        return;
    qWarning("Drop on %s", text(i).latin1());
    QStringList fileList;
    if(!QUriDrag::decodeToUnicodeUris(ev, fileList)){
        qWarning("Pixie: Can't decode drop.");
        return;
    }
    if(!fileList.count())
        return;
    int op;
    QPopupMenu opPopup;
    opPopup.insertItem(i18n("&Copy Here"), 1);
    opPopup.insertItem(i18n("&Move Here"), 2);
    opPopup.insertItem(i18n("&Link Here"), 3);
    QPoint pos = contentsToViewport(ev->pos());
    op = opPopup.exec(viewport()->mapToGlobal(pos));
    switch(op){
    case 1:
        ev->setAction(QDropEvent::Copy);
        break;
    case 2:
        ev->setAction(QDropEvent::Move);
        break;
    case 3:
        ev->setAction(QDropEvent::Link);
        break;
    default:
        return;
    }

    emit dropOnList(i, fileList, ev->action());
}

void KIFHotListBox::resizeEvent(QResizeEvent *ev)
{
    QListBox::resizeEvent(ev);
    addBtn->move(viewport()->width()+1-addBtn->width()-delBtn->width(),
                 viewport()->y());
    delBtn->move(viewport()->width()+1-delBtn->width(), viewport()->y());
}

void KIFHotListBox::slotAddClicked()
{
    emit addCWD();
}

void KIFHotListBox::slotDelClicked()
{
    if(currentItem() != -1)
        emit deleteHotItem(currentItem());
}




#include "hotlistbox.moc"

