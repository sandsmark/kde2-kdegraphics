#ifndef __KIF_IMAGELISTITEM_H
#define __KIF_IMAGELISTITEM_H

#include <qlistbox.h>
#include <kurl.h>

class KIFImageListItem : public QListBoxText
{
public:
    KIFImageListItem(QListBox *listbox, const KURL &url);
    ~KIFImageListItem();
    // returns a local filename, downloading it if needed.
    QString fileName();
    void clearTempData();
    KURL url(){return(u);}
protected:
    KURL u;
    QString tempFileStr;
};


#endif

