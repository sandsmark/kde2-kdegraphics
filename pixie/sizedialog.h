#ifndef __KIF_SIZEDIALOG_H
#define __KIF_SIZEDIALOG_H

#include <qdialog.h>
#include <knuminput.h>

class QCheckBox;

class KIFSizeDialog : public QDialog
{
    Q_OBJECT
public:
    KIFSizeDialog(int w, int h, QWidget *parent=0, const char *name=0);
    int resultWidth(){return(wEdit->value());}
    int resultHeight(){return(hEdit->value());}
protected slots:
    void slotWidthChanged(int);
    void slotHeightChanged(int);
protected:
    int origW, origH;
    QCheckBox *aspectCB;
    KIntNumInput *wEdit, *hEdit;
};

#endif
