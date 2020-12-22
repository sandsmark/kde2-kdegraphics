// $Id: depthdialog.cpp 72097 2000-11-25 11:46:20Z mlaurent $

#include <kdebug.h>
#include <qpushbutton.h>
#include <klocale.h>
#include <qlayout.h>
#include <kapp.h>
#include "depthdialog.h"

depthDialog::depthDialog(Canvas *c, QWidget* parent, const char* name)
  : QDialog(parent, name, TRUE)
{
  QPushButton *okButton;
  QPushButton *cancelButton;
  QLabel *tmpLabel;
  setCaption(i18n("Color depth"));

  QVBoxLayout *mainLayout = new QVBoxLayout(this, 20);

  QHBoxLayout *hbl1 = new QHBoxLayout();
  mainLayout->addLayout( hbl1 );

  tmpLabel = new QLabel( this, "Label_7" );
  tmpLabel->setText( i18n("Image Depth:") );
  tmpLabel->setFixedSize( tmpLabel->sizeHint() );
  hbl1->addWidget( tmpLabel );

  depthBox = new QComboBox( FALSE, this, "ComboBox_1" );
  depthBox->setSizeLimit( 10 );
// depthBox->setAutoResize( FALSE );
  depthBox->insertItem(i18n("1 (2 Colors)"), ID_COLOR_1);
  depthBox->insertItem(i18n("4 (16 Colors)"), ID_COLOR_4);
  depthBox->insertItem(i18n("8 (256 Colors)"), ID_COLOR_8);
  depthBox->insertItem(i18n("15 (32k Colors)"), ID_COLOR_15);
  depthBox->insertItem(i18n("16 (64k Colors)"), ID_COLOR_16);
  depthBox->insertItem(i18n("24 (True Color)"), ID_COLOR_24);
  depthBox->insertItem(i18n("32 (True Color)"), ID_COLOR_32);

  depthBox->setFixedSize( depthBox->sizeHint() );
  hbl1->addWidget( depthBox );
  //  depthBox->setCurrentItem(c->pixmap()->depth());
  
  switch (c->pixmap()->depth()) {
  default: // fall to 1bit colordepth
  case 1:
    depthBox->setCurrentItem(ID_COLOR_1);
    break;
  case 4:
    depthBox->setCurrentItem(ID_COLOR_4);
    break;
  case 8:
    depthBox->setCurrentItem(ID_COLOR_8);
    break;
  case 15:
    depthBox->setCurrentItem(ID_COLOR_15);
    break;
  case 16:
    depthBox->setCurrentItem(ID_COLOR_16);
    break;
  case 24:
    depthBox->setCurrentItem(ID_COLOR_24);
    break;
  case 32:
    depthBox->setCurrentItem(ID_COLOR_32);
    break;
  }

  /*
  if (c->pixmap()->depth() == 1)
    depthBox->setCurrentItem(1);
  else if (c->pixmap()->depth() == 8)
    depthBox->setCurrentItem(8);
  else if (c->pixmap()->depth() == 24)
    depthBox->setCurrentItem(24);
  */

  QHBoxLayout *hbl2 = new QHBoxLayout();
  mainLayout->addLayout( hbl2 );

  okButton= new QPushButton(i18n("OK"), this);
  okButton->setFixedSize( okButton->sizeHint() );
  hbl2->addStretch( 1 );
  hbl2->addWidget( okButton );

  cancelButton= new QPushButton(i18n("Cancel"), this);
  cancelButton->setFixedSize( cancelButton->sizeHint() );
  hbl2->addWidget( cancelButton );

  resize( minimumSize() );

  connect(okButton, SIGNAL(clicked()), SLOT(accept()) );
  connect(cancelButton, SIGNAL(clicked()), SLOT(reject()) );
}

#include "depthdialog.moc"
