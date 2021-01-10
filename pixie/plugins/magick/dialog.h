#ifndef __MAGICK_DIALOGS_H
#define __MAGICK_DIALOGS_H

#include <qdialog.h>
#include <knuminput.h>
#include <qcombobox.h>
#include <qsemimodal.h>
#include <kprogress.h>
#include <kcolorbtn.h>

// single integer
class MagickIntegerDlg : public QDialog
{
public:
    MagickIntegerDlg(const QString &descStr, const QString &ctrlStr,
                     int min, int max, int def, QWidget *parent=0,
                     const char *name=0);
    int value(){return(input->value());}
protected:
    KIntNumInput *input;
};

// two integers
class MagickDblIntegerDlg : public QDialog
{
public:
    MagickDblIntegerDlg(const QString &descStr, const QString &i1Str,
                        int min1, int max1, int def1, const QString &i2Str,
                        int min2, int max2, int def2, QWidget *parent=0,
                        const char *name=0);
    int value1(){return(input1->value());}
    int value2(){return(input2->value());}
protected:
    KIntNumInput *input1, *input2;
};

// Double value
class MagickDoubleDlg : public QDialog
{
public:
    MagickDoubleDlg(const QString &descStr, const QString &ctrlStr,
                    double min, double max, double def, QWidget *parent=0,
                    const char *name=0);
    double value(){return(input->value());}
protected:
    KDoubleNumInput *input;
};

// a color and an integer
class MagickColorIntegerDlg : public QDialog
{
public:
    MagickColorIntegerDlg(const QString &descStr, const QString &ctrlStr,
                          int min, int max, int def,
                          const QString &colorStr, QWidget *parent=0,
                          const char *name=0);
    int value(){return(input->value());}
    QColor color(){return(colorBtn->color());}
protected:
    KIntNumInput *input;
    KColorButton *colorBtn;
};

// a combobox
class MagickComboDlg : public QDialog
{
public:
    MagickComboDlg(const QString &descStr, const QString &comboStr,
                   QWidget *parent=0, const char *name=0);
    int value(){return(combo->currentItem());}
    void insert(const QString &text, int idx=-1){combo->insertItem(text, idx);}
protected:
    QComboBox *combo;
};

class MagickProgressDlg : public QSemiModal
{
public:
    MagickProgressDlg(QWidget *parent=0, const char *name=0);
    void setValue(int val){p->setValue(val);}
protected:
    KProgress *p;
};

        


#endif

