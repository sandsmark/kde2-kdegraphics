#ifndef __THUMB_TOPLEVEL_H
#define __THUMB_TOPLEVEL_H

#include <kmainwindow.h>
#include <qwidget.h>
#include <qdir.h>
#include <qiconview.h>
#include <thumbiconview.h>

class KIFThumbPreview;
class KIFDirTree;
class KIFHotListBox;
class QListBoxItem;

class QListViewItem;
class KProgress;
class QComboBox;
class QPopupMenu;
class QPushButton;
class QLineEdit;
class KPopupMenu;

class KIFThumbTopLevel : public KMainWindow
{
    Q_OBJECT
public:
    KIFThumbTopLevel(const char *name=0);
protected slots:
    void slotPathEdit();
    void slotTreeClicked(QListViewItem *i);
    void slotSortMenu(int id);
    void slotSizeMenu(int id);
    void slotImageOnly();
    void slotUpdateClicked();
    void slotPreviewStatClicked();
    void slotItemClicked(QIconViewItem *i);
    void slotRightButtonPressed(QIconViewItem *i, const QPoint &pos);
    void slotItemDblClicked(QIconViewItem *i);
    void slotSetProgress(int val);
    void slotRightButtonMenu(int id);
    void slotRightButtonDirMenu(int id);
    void slotUpdateView();
    void slotHomeClicked();
    void slotNewWindow();
    void slotUpDir();
    void slotSetStatus(const QString &str);
    void slotHide();
    void slotShow();
    void slotHotList(int id);
    void slotJobFinished(KIO::Job *job);
    void slotHotListBoxClicked(QListBoxItem *item);
    void slotHotListBoxDrop(int id, QStringList &urlList,
                            QDropEvent::Action a);
    void slotAddHotListCWD();
    void slotDelHotList(unsigned int id);
    void slotEnableStop(bool on);
    void slotStopClicked();
signals:
    void fileSelected(const QString &s);
    void fileHighlighted(const QString &s);
    void stopClicked();
protected:
    void closeEvent(QCloseEvent *ev);
    void reloadHotList();
    void addHotListPath(const QString &path);
    void updateStatus(const QString &fileStr, bool ignorePreview=false);
    
    enum ThumbMenuID{TM_Title, TM_Title2, TM_Props, TM_Rename, TM_Delete};
    enum DirMenuID{DM_Title, DM_HotList};
    bool wasVisible;
    KProgress *progress;
    QPopupMenu *sortMenu, *sizeMenu;
    QPopupMenu *optMenu;
    KPopupMenu *rightBtnMnu, *rightBtnDirMnu;
    QStringList hotList;
    QPushButton *previewBtn;
    KIFThumbView *mainWidget;
    KIFThumbPreview *preview;
    KIFHotListBox *hotListBox;
    KIFDirTree *tree;
    QLineEdit *pathEdit;
    QString pathStr, tmpStr;
    int sorting;
    KIFThumbView::IconSize iconSize;
    bool previewStat, dupFirst;
    QIconViewItem *rightClickItem;
};



#endif


