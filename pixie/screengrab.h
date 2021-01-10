#ifndef __KIF_SCREENGRAB_H
#define __KIF_SCREENGRAB_H

#include <qsemimodal.h>
class QCheckBox;
class KIFImageList;

class KIFScreenGrab : public QSemiModal
{
    Q_OBJECT
public:
    KIFScreenGrab(KIFImageList *list, QWidget *parent=0,
                  const char *name=0);
public slots:
    void slotStartGrab();
    void slotCancel();
protected:
    void mousePressEvent(QMouseEvent *ev);
    bool inSelect, winSelected, inLoop;
    QCheckBox *desktopCB;
    QCheckBox *hideCB;
    KIFImageList *imageList;
};

#endif
