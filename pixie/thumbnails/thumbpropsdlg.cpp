#include "thumbpropsdlg.h"
#include <qfileinfo.h>
#include <qlayout.h>
#include <qlabel.h>
#include <kbuttonbox.h>
#include <klocale.h>

KIFThumbProperties::KIFThumbProperties(const QPixmap &pixmap,
                                       const QString &fileStr, QWidget *parent,
                                       const char *name)
    : QDialog(parent, name, true)
{
    QGridLayout *layout = new QGridLayout(this, 1, 1, 4);
    QString tmpStr;
    QFileInfo fi(fileStr);

    QLabel *pixLbl = new QLabel(this);
    pixLbl->setPixmap(pixmap);
    layout->addMultiCellWidget(pixLbl, 0, 4, 0, 0);
    
    layout->addWidget(new QLabel(i18n("Name:"), this) , 0, 1);
    layout->addWidget(new QLabel(i18n("Size:"), this), 1, 1);
    layout->addWidget(new QLabel(i18n("Last modified:"), this), 2, 1);
    layout->addWidget(new QLabel(i18n("Last read on:"), this), 3, 1);

    
    layout->addWidget(new QLabel(fi.fileName(), this) , 0, 2);
    if(fi.size() > 1024)
        tmpStr.sprintf("%.02fK", ((float)fi.size())/1024);
    else
        tmpStr.sprintf("%d bytes", fi.size());
    layout->addWidget(new QLabel(tmpStr, this), 1, 2);
    layout->addWidget(new QLabel(fi.lastModified().toString(), this), 2, 2);
    layout->addWidget(new QLabel(fi.lastRead().toString(), this), 3, 2);

    layout->setRowStretch(4, 1);
    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch(1);
    connect(bbox->addButton(i18n("OK")), SIGNAL(clicked()), this,
            SLOT(accept()));
    layout->addMultiCellWidget(bbox, 5, 5, 0, 2);
    resize(sizeHint());
    setCaption(i18n("Image Properties"));
};

