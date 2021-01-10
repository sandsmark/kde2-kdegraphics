#ifndef __THUMB_DIRTREE_H
#define __THUMB_DIRTREE_H

#include <qlistview.h>
#include <qpixmap.h>

class KIFDirItem : public QListViewItem
{
public:
    KIFDirItem(QListView *parent, const QString &file);
    KIFDirItem(KIFDirItem *parent, const QString &file);
    void setOpen(bool open);
    QString fullPath();
    QString text(int col) const;
protected:
    KIFDirItem *parentDir;
    bool hasAccess;
    QString fileStr;
};

class KIFDirTree : public QListView
{
    Q_OBJECT
public:
    KIFDirTree(QWidget *parent=0, const char *name=0);
    void setPath(const QString &path);
protected slots:
    void slotClicked(QListViewItem *i);
protected:
    QListViewItem *lastOpen;
    QPixmap fPix, fOpenPix, fNoPermPix;

};

#endif

