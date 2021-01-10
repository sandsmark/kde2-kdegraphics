#include "thumbtree.h"

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <qdir.h>
#include <qfile.h>

KIFDirItem::KIFDirItem(KIFDirItem *parent, const QString &file)
    : QListViewItem(parent)
{
    parentDir = parent;
    fileStr = file;
    QDir d(fullPath());
    hasAccess = d.isReadable();
    
    if(hasAccess)
        setPixmap(0, SmallIcon("folder"));
    else
        setPixmap(0, SmallIcon("folder_locked"));
    //setExpandable(true);
}

QString KIFDirItem::text(int) const
{
    return(fileStr);
}


KIFDirItem::KIFDirItem(QListView *parent, const QString &file)
    : QListViewItem(parent)
{
    parentDir = NULL;
    fileStr = file;
    QDir d(fullPath());
    hasAccess = d.isReadable();
    //setExpandable(true);
}

void KIFDirItem::setOpen(bool open)
{
    if(open)
        setPixmap(0, SmallIcon("folder_open"));
    else
        setPixmap(0, SmallIcon("folder"));
    if(open && !childCount()){
        QString pathStr(fullPath());
        QDir currentDir(pathStr);
        currentDir.setFilter(QDir::Dirs);
        if(!currentDir.isReadable()){
            hasAccess = false;
            setExpandable(false);
            return;
        }
        listView()->setUpdatesEnabled(false);
        const QFileInfoList * fileList = currentDir.entryInfoList();
        if(fileList){
            QFileInfoListIterator it(*fileList);
            QFileInfo *fi;
            for(; (fi=it.current()); ++it){
                if (fi->fileName() == "." || fi->fileName() == "..")
                    ;
                else
                    (void)new KIFDirItem(this, fi->fileName() );
            }
        }
	listView()->setUpdatesEnabled( TRUE );
    }
    QListViewItem::setOpen(open);
}

QString KIFDirItem::fullPath()
{
    QString str;
    if (parentDir)
        str = parentDir->fullPath() + fileStr + "/";
    else
	str = fileStr;
    return(str);
}

KIFDirTree::KIFDirTree(QWidget *parent, const char *name)
    : QListView(parent, name)
{
    connect(this, SIGNAL(doubleClicked(QListViewItem *)), this,
            SLOT(slotClicked(QListViewItem *)));
    connect(this, SIGNAL(returnPressed(QListViewItem *)), this,
            SLOT(slotClicked(QListViewItem *)));
    addColumn(i18n("Directory Tree"));
    setTreeStepSize(20);

    KIFDirItem *item = new KIFDirItem(this, "/");
    item->setOpen(true);
}

void KIFDirTree::slotClicked(QListViewItem *i)
{
    KIFDirItem *item = (KIFDirItem *)i;

    item->setOpen(true);
    item->repaint();
    qWarning("Clicked folder %s", item->fullPath().latin1());
}

void KIFDirTree::setPath(const QString &path)
{
    QListViewItemIterator it( this );
    for ( ++it; it.current(); ++it ) {
	it.current()->setOpen( FALSE );
    }

    qWarning("In setPath for %s", path.latin1());
    QStringList lst(QStringList::split( "/", path ));
    QListViewItem *item = firstChild();
    QStringList::Iterator it2 = lst.begin();
    QListViewItem *childItem = NULL;

    for( ; it2 != lst.end(); ++it2 ) {
        qWarning("Looking for %s",  (*it2).latin1());
        item->setOpen(true);
        childItem = item->firstChild();
        item = NULL;
        while(childItem){
            if(childItem->text(0) == *it2){
                qWarning("Found match for %s", (*it2).latin1());
                childItem->setOpen(true);
                item = childItem;
                break;
            }
            else
                childItem = childItem->itemBelow();
        }
        if(!item)
            return;
    }
    setCurrentItem(item);
    setSelected(item, true);
    ensureItemVisible(item);
}

#include "thumbtree.moc"

