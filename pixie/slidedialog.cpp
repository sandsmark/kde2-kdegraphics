#include "slidedialog.h"
 
#include <qlayout.h>
#include <qcheckbox.h>
#include <kbuttonbox.h>
#include <klocale.h>
 
KIFSlideDialog::KIFSlideDialog(QWidget *parent, const char *name)
    : QDialog(parent, name, true)
{
    setCaption(i18n("SlideShow"));
    QVBoxLayout *layout = new QVBoxLayout(this, 4);
    delayEdit = new KIntNumInput(5, this);
    delayEdit->setLabel(i18n("Delay in seconds"));
    layout->addWidget(delayEdit);
    loopCB = new QCheckBox(i18n("Loop"), this);
    layout->addWidget(loopCB);

    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch(1);
    connect(bbox->addButton(i18n("Cancel")), SIGNAL(clicked()), this,
            SLOT(reject()));
    connect(bbox->addButton(i18n("OK")), SIGNAL(clicked()), this,
            SLOT(accept()));
    layout->addWidget(bbox);         
}

#include "slidedialog.moc"

