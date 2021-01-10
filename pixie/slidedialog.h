#ifndef __KIF_SLIDEDIALOG_H
#define __KIF_SLIDEDIALOG_H
 
#include <qdialog.h>
#include <knuminput.h>
#include <qcheckbox.h>
 
class QCheckBox;
 
class KIFSlideDialog : public QDialog
{
    Q_OBJECT
public:
    KIFSlideDialog(QWidget *parent=0, const char *name=0);
    bool loop(){return(loopCB->isChecked());}
    int delay(){return(delayEdit->value());}
protected:
    QCheckBox *loopCB;
    KIntNumInput *delayEdit;
};

#endif

