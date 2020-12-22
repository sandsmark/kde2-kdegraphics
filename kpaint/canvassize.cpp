// $Id: canvassize.cpp 140631 2002-03-03 17:24:45Z mlaurent $

#include <kdebug.h>
#include <stdlib.h>
#include <qlayout.h>

#include <klocale.h>
#include <kapp.h>
#include <kglobal.h>
#include <knumvalidator.h>

#include "canvassize.h"

/**
 * A modal dialog to ask for the size of the canvas.
 */
canvasSizeDialog::canvasSizeDialog(QWidget *parent, const char *name,int _width,int _height)
 : KDialogBase (parent, name, true, i18n("New Canvas Size"), Ok | Cancel, Ok, true)
{
  width=_width;
  height=_height;

  QFrame *frame = makeMainWidget();

  // Create the grid for the entry widgets
  QGridLayout *mainLayout= new QGridLayout(frame, 3, 2, 0, spacingHint());

  widthLabel= new QLabel(i18n("Width:"), frame);
  heightLabel= new QLabel(i18n("Height:"), frame);
  widthEdit= new KLineEdit(frame);
  widthEdit->setValidator( new KIntValidator( 1,9999,widthEdit ) );
  widthEdit->setFocus();
  QString tmp;
  widthEdit->setText(tmp.setNum(_width));

  heightEdit= new KLineEdit(frame);
  heightEdit->setValidator( new KIntValidator( 1,9999,heightEdit ) );
  heightEdit->setText(tmp.setNum(_height));
  pictureRatio=new QCheckBox(i18n("Keep picture ratio"),frame);
  pictureRatio->setChecked(false);

  connect(widthEdit, SIGNAL(returnPressed()), SLOT(slotOk()) );
  connect(heightEdit, SIGNAL(returnPressed()), SLOT(slotOk()) );
  connect(widthEdit, SIGNAL(textChanged(const QString &)),this, SLOT(slotWidthTextChanged(const QString &) ));
  connect(pictureRatio,SIGNAL(clicked ()),this,SLOT(changeState()));

  mainLayout->addWidget(widthLabel, 0, 0, AlignCenter);
  widthEdit->setFixedHeight(widthLabel->sizeHint().height()+8);
  mainLayout->addWidget(widthEdit, 0, 1);
  mainLayout->addWidget(heightLabel, 1, 0, AlignCenter);
  heightEdit->setFixedHeight(heightLabel->sizeHint().height()+8);
  mainLayout->addWidget(heightEdit, 1, 1);
  mainLayout->setColStretch(0, 1);
  mainLayout->setColStretch(1, 3);
  mainLayout->addMultiCellWidget(pictureRatio,2,2,0,1);
}

int canvasSizeDialog::getHeight()
{
  return (int)KGlobal::locale()->readNumber(heightEdit->text());
}

int canvasSizeDialog::getWidth()
{
  return (int)KGlobal::locale()->readNumber(widthEdit->text());
}

void canvasSizeDialog::slotWidthTextChanged(const QString &/*_str*/)
{
  if ( pictureRatio->isChecked() ) {
    double ratioinv=(double)height/(double)width;
    int val=getWidth();
    QString tmp;
    tmp.setNum((int)( ratioinv*val ));
    heightEdit->setText(tmp);
  }
}

void canvasSizeDialog::changeState()
{
  if(pictureRatio->isChecked())
    heightEdit->setEnabled(false);
  else
    heightEdit->setEnabled(true);
}

#include "canvassize.moc"
