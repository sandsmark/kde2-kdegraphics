// $Id: infodialog.cpp 107633 2001-07-25 15:35:16Z rich $

#include <qgrid.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
//#include <qlineedit.h>
#include <qpushbutton.h>

#include <kapp.h>
#include <kdebug.h>
#include <klocale.h>
#include <kseparator.h>

#include "canvas.h"

#include "infodialog.h"

imageInfoDialog::imageInfoDialog(Canvas *c, QWidget* parent, const char* name)
  : KDialog(parent, name, TRUE)
{
  setCaption(i18n("Information"));
  QVBoxLayout *mainLayout = new QVBoxLayout( this, marginHint(), spacingHint() );

  //
  // Title block
  //
  QLabel *titleLabel = new QLabel( i18n( "Information" ), 
				   this, "titleLabel" );
  QFont titleFont = titleLabel->font();
  titleFont.setBold( true );
  titleLabel->setFont( titleFont );
  KSeparator *titleSep = new KSeparator( this );
  titleSep->setFrameStyle( QFrame::HLine|QFrame::Plain );
  mainLayout->addWidget( titleLabel );
  mainLayout->addWidget( titleSep );

  //
  // Content
  //
  QGrid *info = new QGrid( 2, this );
  info->setSpacing( spacingHint() );
  mainLayout->addWidget( info );

  //
  // Colour depth
  //
  QLabel *tmpQLabel = new QLabel( info, "Label_3" );
  tmpQLabel->setText( i18n( "Color Depth:" ) );

  colourDepth = new QLabel( info, "Label_4" );
  QString depthStr;
  depthStr.setNum( c->pixmap()->depth() );
  colourDepth->setText( depthStr );
  colourDepth->setAlignment( AlignRight );

  //
  // Colours used
  //
  tmpQLabel = new QLabel( info, "Label_5" );
  tmpQLabel->setText( i18n("Colors Used:") );

  coloursUsed = new QLabel( info, "Label_6" );
  QString colUseStr;
  colUseStr.setNum( c->pixmap()->convertToImage().numColors() );
  coloursUsed->setText( colUseStr );
  coloursUsed->setAlignment( AlignRight );

  //  tmpQLabel = new QLabel( this, "Label_8" );
  //  tmpQLabel->setText( i18n("Transparent Color:") );
  //  QLineEdit* tmpQLineEdit;
  //  tmpQLineEdit = new QLineEdit( this, "LineEdit_1" );

  //
  // Image size
  //
  tmpQLabel = new QLabel( info, "Label_9" );
  tmpQLabel->setText( i18n( "Image Width:" ) );

  QSize sz = c->pixmap()->size();

  width = new QLabel( info, "Label_10" );
  QString widthStr;
  widthStr.setNum( sz.width() );
  width->setText( widthStr );
  width->setAlignment( AlignRight );

  tmpQLabel = new QLabel( info, "Label_11" );
  tmpQLabel->setText( i18n("Image Height:") );

  height = new QLabel( info, "Label_12" );
  QString heightStr;
  heightStr.setNum( sz.height() );
  height->setText( heightStr );
  height->setAlignment( AlignRight );

  //
  // Buttons
  //
  KSeparator *btnSep = new KSeparator( this );
  btnSep->setFrameStyle( QFrame::HLine|QFrame::Plain );
  mainLayout->addWidget( btnSep );

  QBoxLayout *btnLayout = new QBoxLayout( mainLayout, QBoxLayout::RightToLeft );
  QPushButton *okButton= new QPushButton( i18n("Dismiss"), this );
  okButton->setFixedHeight( okButton->sizeHint().height() );
  connect( okButton, SIGNAL(clicked()), SLOT(accept()) );
  btnLayout->addWidget( okButton );
  btnLayout->addStretch( 100 );

  resize( 200, 160 );
}

#include "infodialog.moc"

