#ifndef __THUMB_ICONVIEW_H
#define __THUMB_ICONVIEW_H

#include <qiconview.h>
#include <qdir.h>
#include <kio/job.h>

class KIFThumbViewItem : public QIconViewItem
{
public:
    KIFThumbViewItem(QIconView *parent, const QString &textStr,
                     const QPixmap &pix, bool isParent=false)
        : QIconViewItem(parent, textStr, pix){parentFlag = isParent;}
    bool acceptDrop(const QMimeSource *) const;
    bool parentDir(){return(parentFlag);}
protected:
    void dropped(QDropEvent *ev, const QValueList<QIconDragItem> &);
    bool parentFlag;
};
    

class KIFThumbView : public QIconView
{
    Q_OBJECT
public:
    enum IconSize{Small=0, Medium, Large};
    KIFThumbView(QWidget *parent=0, const char *name=0);
    ~KIFThumbView();
    // dupFirst valid only on size sort mode!
    void update(const QString &dir, IconSize iSize, int sortType,
                bool createThumbs = false, bool dupFirst = false);
    QString currentPath() {return(currentDir);}
    void setIconOnly(bool en);
public slots:
    void slotStopClicked();
signals:
    void updateProgress(int val);
    void updateMe();
    void enableStopButton(bool on);
protected slots:
    void slotIconDrop(QDropEvent *ev, const QValueList<QIconDragItem> &);
    void slotDropJobFinished(KIO::Job *job);
    void slotDropJobReload(KIO::Job *job);
protected:
    // make sure to delete result
    QFileInfoList* dupSizeFirst(const QFileInfoList *src);
    virtual void drawBackground(QPainter *p, const QRect &r);
    bool isImageExtension(const QString &fileStr);
    QDragObject* dragObject();

    bool iconOnly, stopProcessing;
    QString currentDir;
    QBrush wallBrush;
};



#endif

