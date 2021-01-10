#ifndef __KIF_HOTLISTBOX_H
#define __KIF_HOTLISTBOX_H

#include <qlistbox.h>
class QToolButton;

class KIFHotListBox : public QListBox
{
    Q_OBJECT
public:
    KIFHotListBox(QWidget *parent=0, const char *name=0);
signals:
    void addCWD();
    void deleteHotItem(unsigned int id);
    void dropOnList(int id, QStringList &urlList, QDropEvent::Action action);
protected slots:
    void slotAddClicked();
    void slotDelClicked();
protected:
    void dragEnterEvent(QDragEnterEvent *ev);
    void dragLeaveEvent(QDragLeaveEvent *ev);
    void dragMoveEvent(QDragMoveEvent *ev);
    void dropEvent(QDropEvent *ev);
    void resizeEvent(QResizeEvent *ev);

    QListBoxItem *lastDndHighlight;
    QToolButton *addBtn, *delBtn;
};

#endif


