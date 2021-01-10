#include "dndlistbox.h"
#include <qdragobject.h>

#include <kurl.h>
#include <kdebug.h>

void KIFDndListBox::dragEnterEvent(QDragEnterEvent *ev)
{
    ev->accept(QUriDrag::canDecode(ev));
}

void KIFDndListBox::dropEvent(QDropEvent *ev)
{
    QStrList fileList;
    bool validUrls = false;
    if(QUriDrag::decode(ev, fileList)){
        QStrListIterator it(fileList);
        for(;it.current(); ++it){
            kdDebug() << "In dropEvent for " << it.current() << endl;
            KURL url(it.current());
            if(!url.isMalformed()){
                validUrls = true;
                emit urlDropped(it.current());
            }
        }
        if(validUrls)
            emit dropFinished();
    }
}

#include "dndlistbox.moc"

