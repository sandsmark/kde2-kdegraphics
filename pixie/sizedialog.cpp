#include "sizedialog.h"

#include <qlayout.h>
#include <qcheckbox.h>
#include <kbuttonbox.h>
#include <klocale.h>

KIFSizeDialog::KIFSizeDialog(int w, int h, QWidget *parent, const char *name)
    : QDialog(parent, name, true)
{
    QVBoxLayout *layout = new QVBoxLayout(this, 4);
    wEdit = new KIntNumInput(w, this);
    wEdit->setLabel(i18n("Width"));
    wEdit->setRange(2, 6000);
    connect(wEdit, SIGNAL(valueChanged(int)), this,
            SLOT(slotWidthChanged(int)));
    layout->addWidget(wEdit);
    hEdit = new KIntNumInput(wEdit, h, this);
    hEdit->setLabel(i18n("Height"));
    hEdit->setRange(2, 6000);
    connect(hEdit, SIGNAL(valueChanged(int)), this,
            SLOT(slotHeightChanged(int)));
    layout->addWidget(hEdit);

    /*aspectCB = new QCheckBox(i18n("Maintain aspect ratio."), this);
    aspectCB->setChecked(true);
    layout->addWidget(aspectCB);*/
    layout->addStretch(1);

    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch(1);
    connect(bbox->addButton(i18n("Cancel")), SIGNAL(clicked()), this,
            SLOT(reject()));
    connect(bbox->addButton(i18n("OK")), SIGNAL(clicked()), this,
            SLOT(accept()));
    layout->addWidget(bbox);
    setCaption(i18n("Image Size"));
    origW = w;
    origH = h;
}

void KIFSizeDialog::slotWidthChanged(int)
{

}

void KIFSizeDialog::slotHeightChanged(int)
{

}

#include "sizedialog.moc"



