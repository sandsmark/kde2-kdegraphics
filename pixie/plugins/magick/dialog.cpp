#include "dialog.h"
#include <qlineedit.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <qlayout.h>
#include <kseparator.h>
#include <kbuttonbox.h>
#include <klocale.h>

MagickIntegerDlg::MagickIntegerDlg(const QString &descStr,
                                   const QString &ctrlStr, int min,
                                   int max, int def, QWidget *parent,
                                   const char *name)
    : QDialog(parent, name, true)
{
    QVBoxLayout *layout = new QVBoxLayout(this, 4);
    layout->addSpacing(10);
    QLabel *lbl = new QLabel(descStr, this);
    layout->addWidget(lbl);
    layout->addSpacing(32);
    input = new KIntNumInput(def, this, 10, ctrlStr.ascii());
    input->setRange(min, max);
    layout->addWidget(input);
    layout->addSpacing(10);
    layout->addStretch(1);
    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch();
    connect(bbox->addButton(i18n("OK")), SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(bbox->addButton(i18n("Cancel")), SIGNAL(clicked()),
            this, SLOT(reject()));
    layout->addWidget(bbox);
}

MagickDoubleDlg::MagickDoubleDlg(const QString &descStr,
                                 const QString &ctrlStr, double min,
                                 double max, double def, QWidget *parent,
                                 const char *name)
    : QDialog(parent, name, true)
{
    QVBoxLayout *layout = new QVBoxLayout(this, 4);
    layout->addSpacing(10);
    QLabel *lbl = new QLabel(descStr, this);
    layout->addWidget(lbl);
    layout->addSpacing(32);
    input = new KDoubleNumInput(def, this, ctrlStr.ascii());
    input->setRange(min, max, 0.01);
    input->setLabel(ctrlStr);
    layout->addWidget(input);
    layout->addSpacing(10);
    layout->addStretch(1);
    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch();
    connect(bbox->addButton(i18n("OK")), SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(bbox->addButton(i18n("Cancel")), SIGNAL(clicked()),
            this, SLOT(reject()));
    layout->addWidget(bbox);
}

MagickDblIntegerDlg::MagickDblIntegerDlg(const QString &descStr,
                                      const QString &i1Str,
                                      int min1, int max1, int def1,
                                      const QString &i2Str,
                                      int min2, int max2, int def2,
                                      QWidget *parent, const char *name)
    : QDialog(parent, name, true)
{
    QVBoxLayout *layout = new QVBoxLayout(this, 4);
    layout->addSpacing(10);
    QLabel *lbl = new QLabel(descStr, this);
    layout->addWidget(lbl);
    layout->addSpacing(32);
    input1 = new KIntNumInput(def1, this, 10, i1Str.ascii());
    input1->setRange(min1, max1);
    layout->addWidget(input1);
    input2 = new KIntNumInput(def2, this, 10, i2Str.ascii());
    input2->setRange(min2, max2);
    layout->addWidget(input2);
    layout->addSpacing(10);
    layout->addStretch(1);
    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch();
    connect(bbox->addButton(i18n("OK")), SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(bbox->addButton(i18n("Cancel")), SIGNAL(clicked()),
            this, SLOT(reject()));
    layout->addWidget(bbox);
}

MagickColorIntegerDlg::MagickColorIntegerDlg(const QString &descStr,
                                             const QString &ctrlStr,
                                             int min, int max, int def,
                                             const QString &colorStr,
                                             QWidget *parent, const char *name)
    : QDialog(parent, name, true)
{
    QGridLayout *layout = new QGridLayout(this, 1, 1, 4);
    layout->addRowSpacing(0, 10);
    QLabel *lbl = new QLabel(descStr, this);
    layout->addMultiCellWidget(lbl, 1, 1, 0, 2);
    layout->addRowSpacing(2, 32);
    lbl = new QLabel(colorStr, this);
    layout->addWidget(lbl, 3, 0);
    colorBtn = new KColorButton(this);
    layout->addWidget(colorBtn, 3, 1);
    input = new KIntNumInput(def, this, 10, ctrlStr.ascii());
    input->setRange(min, max);
    layout->addMultiCellWidget(input, 4, 4, 0, 2);
    layout->addRowSpacing(5, 10);
    layout->setRowStretch(5, 1);
    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch();
    connect(bbox->addButton(i18n("OK")), SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(bbox->addButton(i18n("Cancel")), SIGNAL(clicked()),
            this, SLOT(reject()));
    layout->addMultiCellWidget(bbox, 6, 6, 0, 2);
    layout->setColStretch(2, 1);
}

MagickComboDlg::MagickComboDlg(const QString &descStr,
                               const QString &comboStr, QWidget *parent,
                               const char *name)
    : QDialog(parent, name, true)
{
    QGridLayout *layout = new QGridLayout(this, 1, 1, 4);
    layout->addRowSpacing(0, 10);
    QLabel *lbl = new QLabel(descStr, this);
    layout->addMultiCellWidget(lbl, 1, 1, 0, 2);
    layout->addRowSpacing(2, 32);
    lbl = new QLabel(comboStr, this);
    layout->addWidget(lbl, 3, 0);
    combo = new QComboBox(this);
    layout->addWidget(combo, 3, 1);
    layout->addRowSpacing(4, 10);
    layout->setRowStretch(4, 1);
    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch();
    connect(bbox->addButton(i18n("OK")), SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(bbox->addButton(i18n("Cancel")), SIGNAL(clicked()),
            this, SLOT(reject()));
    layout->addMultiCellWidget(bbox, 5, 5, 0, 2);
    layout->setColStretch(2, 1);
}


MagickProgressDlg::MagickProgressDlg(QWidget *parent, const char *name)
: QSemiModal(parent, name, true)
{
    QVBoxLayout *layout = new QVBoxLayout(this, 4);
    QLabel *label = new QLabel(i18n("Processing image..."), this);
    layout->addWidget(label);
    layout->addSpacing(20);
    p = new KProgress(0, 100, 0, KProgress::Horizontal, this);
    layout->addWidget(p);
    layout->addSpacing(20);
    resize(sizeHint());
}


