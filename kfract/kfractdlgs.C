    /*

    kfractdlgs.C  version 0.1.2

    Copyright (C) 1997 Uwe Thiem   
                       uwe@uwix.alt.na

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */


#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <kapp.h>
#include <kglobal.h>
#include <klocale.h>

#include <stdio.h>

#include "drawview.h"
#include "kfractdlgs.moc"


#define MAX_ITER 5000
#define PAGE_STEPS  10


static void setDblValue( double val, KRestrictedLine *control,
			int prec = 20 )
{
    QString text;
    text = KGlobal::locale()->formatNumber( val, prec );
    control->setText( text );
}

static void readDblValue( double& val, KRestrictedLine *control,
			int prec = 20 )
{
  bool ok;
  double newval = KGlobal::locale()->readNumber(control->text(), &ok );
  if ( ok )
  {
    val = newval;
  }
  else
  {
    setDblValue( val, control, prec );
  }
}


TypeDlg::TypeDlg( int iter, int iter_default,
                  double bailout, double bailout_default,
                  double center_x, double center_x_default,
                  double center_y, double center_y_default,
                  double actual_width, double width_default,
                  double extra_x, double extra_x_default,
                  double extra_y, double extra_y_default,
                  QWidget *parent, const char *name ) :
         QDialog( parent, name, TRUE )
  {
  QLabel *text_iter, *text_iter_1, *text_iter_1000;
  QLabel *text_center_x, *text_center_y;
  QLabel *text_bailout, *text_width;
  QPushButton *ok, *cancel, *defaultb;

  my_iter = iter;
  my_iter_default = iter_default;
  my_bailout = bailout;
  my_bailout_default = bailout_default;
  my_center_x = center_x;
  my_center_x_default = center_x_default;
  my_center_y = center_y;
  my_center_y_default = center_y_default;
  my_width = actual_width;
  my_width_default = width_default;
  my_extra_x = extra_x;
  my_extra_x_default = extra_x_default;
  my_extra_y = extra_y;
  my_extra_y_default = extra_y_default;

  vLayout = new QVBoxLayout( this, 5 );
  QHBoxLayout *hLayout = new QHBoxLayout();

  vLayout->addLayout( hLayout, 2 );

  text_iter = new QLabel( this );
  CHECK_PTR( text_iter );
  text_iter->setText( i18n("Iterations:") );
  text_iter->setMinimumSize( text_iter->sizeHint() );
  hLayout->addWidget( text_iter, 2 );
  hLayout->addSpacing( 5 );
  hLayout->addStretch( 1 );
  num_iter = new QLabel( this );
  CHECK_PTR( num_iter );
  num_iter->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  num_iter->setText( "100000" );
  num_iter->setMinimumSize( num_iter->sizeHint() );
  num_iter->setNum( my_iter );
  hLayout->addWidget( num_iter, 2 );

  text_iter_1 = new QLabel( this );
  CHECK_PTR( text_iter_1 );
  text_iter_1->setText( i18n("0 - 999") );
  vLayout->addWidget( text_iter_1, 2, AlignLeft );

  if ( my_iter < 1000 )
    { 
    bar_iter_1 = new QScrollBar( 1, 999, 1, PAGE_STEPS, 
                                 my_iter % 1000, 
                                 QScrollBar::Horizontal, this );
    }
  else
    {
    bar_iter_1 = new QScrollBar( 0, 999, 1, PAGE_STEPS,
                                 my_iter % 1000,
                                 QScrollBar::Horizontal, this );
    }
  CHECK_PTR( bar_iter_1 );
  vLayout->addWidget( bar_iter_1, 2 );
  connect( bar_iter_1, SIGNAL( valueChanged( int ) ),
           this, SLOT( myIter1Changed( int ) ) );
  text_iter_1000 = new QLabel( this );
  text_iter_1000->setText( i18n("1000 - 100000") );
  text_iter_1000->setMinimumSize( text_iter_1000->sizeHint() );
  vLayout->addWidget( text_iter_1000, 2, AlignLeft );
  bar_iter_1000 = new QScrollBar( 0, 100, 1, 1,
                                  ( my_iter - ( my_iter % 1000 ) ) / 1000,
                                  QScrollBar::Horizontal, this );
  CHECK_PTR( bar_iter_1000 );
  vLayout->addWidget( bar_iter_1000, 2 );
  connect( bar_iter_1000, SIGNAL( valueChanged( int ) ),
           this, SLOT( myIter1000Changed( int ) ) ); 

  vLayout->addSpacing( 8 );
  vLayout->addStretch( 1 );

  text_bailout = new QLabel( this );
  CHECK_PTR( text_bailout );
  text_bailout->setText( i18n("Bailout") );
  text_bailout->setMinimumSize( text_bailout->sizeHint() );
  vLayout->addWidget( text_bailout, 2, AlignLeft );
  bailout_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( bailout_input );
  bailout_input->setMaxLength( 40 );
  setDblValue( my_bailout, bailout_input );
  bailout_input->setMinimumSize( bailout_input->sizeHint() );
  vLayout->addWidget( bailout_input, 2 );
  connect( bailout_input, SIGNAL( returnPressed() ),
           this, SLOT( setBailout() ) );

  vLayout->addSpacing( 8 );
  vLayout->addStretch( 1 );

  text_center_x = new QLabel( this );
  CHECK_PTR( text_center_x );
  text_center_x->setText( i18n("Center of picture: Real part") );
  text_center_x->setMinimumSize( text_center_x->sizeHint() );
  vLayout->addWidget( text_center_x, 2, AlignLeft );
  center_x_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( center_x_input );
  center_x_input->setMaxLength( 40 );
  setDblValue( my_center_x, center_x_input );
  center_x_input->setMinimumSize( center_x_input->sizeHint() );
  vLayout->addWidget( center_x_input, 2 );
  connect( center_x_input, SIGNAL( returnPressed() ),
           this, SLOT ( setCenterX() ) );
  text_center_y = new QLabel( this );
  CHECK_PTR( text_center_y );
  text_center_y->setText( i18n("Center of picture: Imaginary part") );
  text_center_y->setMinimumSize( text_center_y->sizeHint() );
  vLayout->addWidget( text_center_y, 2, AlignLeft );
  center_y_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( center_y_input );
  center_y_input->setMaxLength( 40 );
  setDblValue( my_center_y, center_y_input );
  center_y_input->setMinimumSize( center_y_input->sizeHint() );
  vLayout->addWidget( center_y_input, 2 );
  connect( center_y_input, SIGNAL( returnPressed() ),
           this, SLOT( setCenterY() ) );

  vLayout->addSpacing( 8 );
  vLayout->addStretch( 1 );

  text_width = new QLabel( this );
  CHECK_PTR( text_width );
  text_width->setText( i18n("Width of picture:") );
  text_width->setMinimumSize( text_width->sizeHint() );
  vLayout->addWidget( text_width, 2, AlignLeft );
  width_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( width_input );
  width_input->setMaxLength( 40 );
  setDblValue( my_width, width_input );
  width_input->setMinimumSize( width_input->sizeHint() );
  vLayout->addWidget( width_input, 2 );
  connect( width_input, SIGNAL( returnPressed() ),
           this, SLOT( setWidth() ) );

  vLayout->addSpacing( 8 );
  vLayout->addStretch( 1 );

  hLayout = new QHBoxLayout();
  vLayout-> addLayout( hLayout, 2 );

  ok = new QPushButton( this );
  CHECK_PTR( ok );
  ok->setText( i18n("OK") );
  connect( ok, SIGNAL( clicked() ), SLOT( typeAccept() ) );
  defaultb = new QPushButton( this );
  CHECK_PTR( defaultb );
  defaultb->setText( i18n("Defaults") );
  connect( defaultb, SIGNAL( clicked() ), SLOT( typeDefaults() ) );
  cancel = new QPushButton( this );
  CHECK_PTR( cancel );
  cancel->setText( i18n("Cancel") );
  connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );
  QSize buttonSize = ok->sizeHint();
  if ( defaultb->sizeHint().width() > buttonSize.width() )
         buttonSize = defaultb->sizeHint();
  if ( cancel->sizeHint().width() > buttonSize.width() )
         buttonSize = cancel->sizeHint();
  ok->setMinimumSize( buttonSize );
  hLayout->addWidget( ok, 2 );
  hLayout->addSpacing( 5 );
  hLayout->addStretch( 1 );
  defaultb->setMinimumSize( buttonSize );
  hLayout->addWidget( defaultb, 2 );
  hLayout->addSpacing( 5 );
  hLayout->addStretch( 1 );
  cancel->setMinimumSize( buttonSize );
  hLayout->addWidget( cancel, 2 );

//  adjustSize();
//  setFixedSize( width(), height() );
  }


void TypeDlg::myIter1Changed( int new_iter )
  {
  my_iter = my_iter - ( my_iter % 1000 ) + new_iter;
  num_iter->setNum( my_iter );
  }


void TypeDlg::myIter1000Changed( int new_iter )
  {
  if ( new_iter < 1 )
    {
    bar_iter_1->setRange( 1, 999 );
    if ( my_iter == 0 )
      {
      my_iter++;
      }
    }
  else
    {
    bar_iter_1->setRange( 0, 999 );
    }
  my_iter = (my_iter % 1000 ) + new_iter * 1000;
  num_iter->setNum( my_iter );
  }


void TypeDlg::typeDefaults()
  {
  my_iter = my_iter_default;
  num_iter->setNum( my_iter );
  bar_iter_1->setValue( my_iter % 1000 );
  bar_iter_1000->setValue( ( my_iter - ( my_iter % 1000 ) ) / 1000 );
  if ( my_iter < 1000 )
    {
    bar_iter_1->setRange( 1, 999 );
    }
  else
    {
    bar_iter_1->setRange( 0, 999 );
    }
  my_bailout = my_bailout_default;
  setDblValue( my_bailout, bailout_input );
  my_center_x = my_center_x_default;
  setDblValue( my_center_x, center_x_input );
  my_center_y = my_center_y_default;
  setDblValue( my_center_y, center_y_input );
  my_width = my_width_default;
  setDblValue( my_width, width_input );
  emit defaultsChosen();
  }


void TypeDlg::typeAccept()
  {
  emit changedType();
  emit iterChanged( my_iter );
  emit bailoutChanged( my_bailout );
  emit centerXChanged( my_center_x );
  emit centerYChanged( my_center_y );
  emit widthChanged( my_width );
  emit okChosen();
  accept();
  }


void TypeDlg::setBailout()
  {
  readDblValue( my_bailout, bailout_input );
  }


void TypeDlg::setCenterX()
  {
  readDblValue( my_center_x, center_x_input );
  }


void TypeDlg::setCenterY()
  {
  readDblValue( my_center_y, center_y_input );
  }


void TypeDlg::setWidth()
  {
  readDblValue( my_width, width_input );
  }






//###################################################################

MandelDlg::MandelDlg( int iter, int iter_default,
                      double bailout, double bailout_default,
                      double center_x, double center_x_default,
                      double center_y, double center_y_default,
                      double actual_width, double width_default,
                      QWidget *parent, const char *name ) :
            TypeDlg( iter, iter_default,
                     bailout, bailout_default,
                     center_x, center_x_default,
                     center_y, center_y_default,
                     actual_width, width_default,
                     0.0, 0.0,
                     0.0, 0.0,
                     parent, name )
  {
  QLabel *header1, *header2;

  header1 = new QLabel( this );
  CHECK_PTR( header1 );
  header1->setText( i18n("z(0) = c = pixel") );
  header1->setMinimumSize( header1->sizeHint() );
  vLayout->insertWidget( 0, header1, 2, AlignCenter );
  header2 = new QLabel( this );
  CHECK_PTR( header2 );
  header2->setText( i18n("z(n+1) = z(n)^2 + c") );
  header2->setMinimumSize( header2->sizeHint() );
  vLayout->insertWidget( 1, header2, 2, AlignCenter );
  vLayout->insertSpacing( 2, 8 );
  vLayout->insertStretch( 3, 1 );

  connect( this, SIGNAL( changedType() ),
           this, SLOT( typeHasChanged() ) );
  setCaption( i18n("Mandelbrot") );

  vLayout->activate();
  }



void MandelDlg::typeHasChanged()
  {
  emit( changedToMandel() );
  }





//####################################################################

JuliaDlg::JuliaDlg( int iter, int iter_default,
                    double bailout, double bailout_default,
                    double center_x, double center_x_default,
                    double center_y, double center_y_default,
                    double actual_width, double width_default,
                    double extra_x, double extra_x_default,
                    double extra_y, double extra_y_default,
                    QWidget *parent, const char *name ) :
            TypeDlg( iter, iter_default,
                     bailout, bailout_default,
                     center_x, center_x_default,
                     center_y, center_y_default,
                     actual_width, width_default,
                     extra_x, extra_x_default,
                     extra_y, extra_y_default,
                     parent, name )
  {
  QLabel *header1, *header2;
  QLabel *text_extra_x, *text_extra_y;

  my_extra_x = extra_x;
  my_extra_x_default = extra_x_default;
  my_extra_y = extra_y;
  my_extra_y_default = extra_y_default;

  header1 = new QLabel( this );
  CHECK_PTR( header1 );
  header1->setText( i18n("z(0) = pixel") );
  header1->setMinimumSize( header1->sizeHint() );
  vLayout->insertWidget( 0, header1, 2, AlignCenter );
  header2 = new QLabel( this );
  CHECK_PTR( header2 );
  header2->setText( i18n("z(n+1) = z(n)^2 + c") );
  header2->setMinimumSize( header2->sizeHint() );
  vLayout->insertWidget( 1, header2, 2, AlignCenter );

  vLayout->insertSpacing( 2, 8 );
  vLayout->insertStretch( 3, 1 );

  int index = vLayout->findWidget( width_input );
  index += 2;

  text_extra_x = new QLabel( this );
  CHECK_PTR( text_extra_x );
  text_extra_x->setText( i18n("Extra parameter: Real part") );
  text_extra_x->setMinimumSize( text_extra_x->sizeHint() );
  vLayout->insertWidget( index++, text_extra_x, 2, AlignLeft );
  extra_x_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( extra_x_input );
  extra_x_input->setMaxLength( 40 );
  setDblValue( my_extra_x, extra_x_input );
  extra_x_input->setMinimumSize( extra_x_input->sizeHint() );
  vLayout->insertWidget( index++, extra_x_input, 2 );
  connect( extra_x_input, SIGNAL( returnPressed() ),
           this, SLOT ( setExtraX() ) );
  text_extra_y = new QLabel( this );
  CHECK_PTR( text_extra_y );
  text_extra_y->setText( i18n("Extra parameter: Imaginary part") );
  text_extra_y->setMinimumSize( text_extra_y->sizeHint() );
  vLayout->insertWidget( index++, text_extra_y, 2, AlignLeft );
  extra_y_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( extra_y_input );
  extra_y_input->setMaxLength( 40 );
  setDblValue( my_extra_y, extra_y_input );
  extra_y_input->setMinimumSize( extra_y_input->sizeHint() );
  vLayout->insertWidget( index++, extra_y_input, 2 );

  vLayout->insertSpacing( index++, 8 );
  vLayout->insertStretch( index, 2 );

  connect( extra_y_input, SIGNAL( returnPressed() ),
           this, SLOT( setExtraY() ) );

  connect( this, SIGNAL( changedType() ),
           this, SLOT( typeHasChanged() ) );
  setCaption( i18n("Julia") );
  connect( this, SIGNAL( defaultsChosen() ),
           this, SLOT( juliaDefaults() ) );
  connect( this, SIGNAL( okChosen() ),
           this, SLOT( juliaAccept() ) );

  vLayout->activate();
  }



void JuliaDlg::typeHasChanged()
  {
  emit( changedToJulia() );
  }



void JuliaDlg::setExtraX()
  {
  readDblValue( my_extra_x, extra_x_input );
  }


void JuliaDlg::setExtraY()
  {
  readDblValue( my_extra_y, extra_y_input );
  }



void JuliaDlg::juliaDefaults()
  {
  my_extra_x = my_extra_x_default;
  setDblValue( my_extra_x, extra_x_input );
  my_extra_y = my_extra_y_default;
  setDblValue( my_extra_y, extra_y_input );
  }



void JuliaDlg::juliaAccept()
  {
  emit( extraXChanged( my_extra_x ) );
  emit( extraYChanged( my_extra_y ) );
  }



//####################################################################



IterDlg::IterDlg( int iter, int iter_default,
                      QWidget *parent, const char *name ) :
            QDialog( parent, name, TRUE )
  {
  QLabel *text_iter, *text_iter_1, *text_iter_1000;
  QPushButton *ok, *cancel, *defaultb;

  my_iter = iter;
  my_iter_default = iter_default;
  setCaption( i18n("Max. Iterations") );

  QVBoxLayout *vLayout = new QVBoxLayout( this, 5 );
  QHBoxLayout *hLayout = new QHBoxLayout();
  vLayout->addLayout( hLayout, 2 );
  
  text_iter = new QLabel( this );
  CHECK_PTR( text_iter );
  text_iter->setText( i18n("Iterations:") );
  text_iter->setMinimumSize( text_iter->sizeHint() );
  hLayout->addWidget( text_iter, 2 );
  hLayout->addSpacing( 5 );
  hLayout->addStretch( 1 );
  num_iter = new QLabel( this );
  CHECK_PTR( num_iter );
  num_iter->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  num_iter->setText( "100000" );
  num_iter->setMinimumSize( num_iter->sizeHint() );
  num_iter->setNum( my_iter );
  hLayout->addWidget( num_iter, 2 );
  text_iter_1 = new QLabel( this );
  CHECK_PTR( text_iter_1 );
  text_iter_1->setText( i18n("0 - 999") );
  text_iter_1->setMinimumSize( text_iter_1->sizeHint() );
  vLayout->addWidget( text_iter_1, 2, AlignLeft );
  if ( my_iter < 1000 )
    {
    bar_iter_1 = new QScrollBar( 1, 999, 1, PAGE_STEPS,
                                 my_iter % 1000,
                                 QScrollBar::Horizontal, this );
    }
  else
    {
    bar_iter_1 = new QScrollBar( 0, 999, 1, PAGE_STEPS,
                                 my_iter % 1000,
                                 QScrollBar::Horizontal, this );
    }
  CHECK_PTR( bar_iter_1 );
  vLayout->addWidget( bar_iter_1, 2 );
  connect( bar_iter_1, SIGNAL( valueChanged( int ) ),
           this, SLOT( myIter1Changed( int ) ) );
  text_iter_1000 = new QLabel( this );
  text_iter_1000->setText( i18n("1000 - 100000") );
  text_iter_1000->setMinimumSize( text_iter_1000->sizeHint() );
  vLayout->addWidget( text_iter_1000, 2, AlignLeft );
  bar_iter_1000 = new QScrollBar( 0, 100, 1, 1,
                                  ( my_iter - ( my_iter % 1000 ) ) / 1000,
                                  QScrollBar::Horizontal, this );
  CHECK_PTR( bar_iter_1000 );
  vLayout->addWidget( bar_iter_1000, 2 );
  connect( bar_iter_1000, SIGNAL( valueChanged( int ) ),
           this, SLOT( myIter1000Changed( int ) ) ); 

  vLayout->addSpacing( 8 );
  vLayout->addStretch( 1 );

  hLayout = new QHBoxLayout();
  vLayout->addLayout( hLayout, 2 );

  ok = new QPushButton( this );
  CHECK_PTR( ok );
  ok->setText( i18n("OK") );
  connect( ok, SIGNAL( clicked() ), SLOT( iterAccept() ) );
  defaultb = new QPushButton( this );
  CHECK_PTR( defaultb );
  defaultb->setText( i18n("Defaults") );
  connect( defaultb, SIGNAL( clicked() ), SLOT( iterDefaults() ) );
  cancel = new QPushButton( this );
  CHECK_PTR( cancel );
  cancel->setText( i18n("Cancel") );
  connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );
  QSize buttonSize = ok->sizeHint();
  if ( defaultb->sizeHint().width() > buttonSize.width() )
           buttonSize = defaultb->sizeHint();
  if ( cancel->sizeHint().width() > buttonSize.width() )
           buttonSize = cancel->sizeHint();
  ok->setMinimumSize( buttonSize );
  hLayout->addWidget( ok, 2 );
  hLayout->addSpacing( 5 );
  hLayout->addStretch( 1 );
  defaultb->setMinimumSize( buttonSize );
  hLayout->addWidget( defaultb, 2 );
  hLayout->addSpacing( 5 );
  hLayout->addStretch( 1 );
  cancel->setMinimumSize( buttonSize );
  hLayout->addWidget( cancel, 2 );

  vLayout->activate();
  }



void IterDlg::myIter1Changed( int new_iter )
  {
  my_iter = my_iter - ( my_iter % 1000 ) + new_iter;
  num_iter->setNum( my_iter );
  }


void IterDlg::myIter1000Changed( int new_iter )
  {
  if ( new_iter < 1 )
    {
    bar_iter_1->setRange( 1, 999 );
    if ( my_iter == 0 )
      {
      my_iter++;
      }
    }
  else
    {
    bar_iter_1->setRange( 0, 999 );
    }
  my_iter = (my_iter % 1000 ) + new_iter * 1000;
  num_iter->setNum( my_iter );
  }


void IterDlg::iterDefaults()
  {
  my_iter = my_iter_default;
  num_iter->setNum( my_iter );
  bar_iter_1->setValue( my_iter % 1000 );
  bar_iter_1000->setValue( ( my_iter - ( my_iter % 1000 ) ) / 1000 );
  if ( my_iter < 1000 )
    {
    bar_iter_1->setRange( 1, 999 );
    }
  else
    {
    bar_iter_1->setRange( 0, 999 );
    }
  }



void IterDlg::iterAccept()
  {
  emit iterChanged( my_iter );
  accept();
  }



//##############################################################

ZoomDlg::ZoomDlg( double zoom_in, double zoom_in_default,
                  double zoom_out, double zoom_out_default,
                  QWidget *parent, const char *name ) :
         QDialog( parent, name, TRUE )
  {
  QPushButton *ok, *defaultb, *cancel;
  QLabel *text_zoom_in, *text_zoom_out;

  my_zoom_in = zoom_in;
  my_zoom_in_default = zoom_in_default;
  my_zoom_out = zoom_out;
  my_zoom_out_default = zoom_out_default;

  QVBoxLayout *vLayout = new QVBoxLayout( this, 5 );
  QHBoxLayout *hLayout = new QHBoxLayout();
  vLayout->addLayout( hLayout, 2 );

  text_zoom_in = new QLabel( this );
  CHECK_PTR( text_zoom_in );
  text_zoom_in->setText( i18n("Zoom in factor:") );
  text_zoom_in->setMinimumSize( text_zoom_in->sizeHint() );
  hLayout->addWidget( text_zoom_in, 2 );
  hLayout->addSpacing( 5 );
  hLayout->addStretch( 1 );
  zoom_in_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( zoom_in_input );
  zoom_in_input->setMaxLength( 20 );
  setDblValue( my_zoom_in, zoom_in_input, 10 );
  zoom_in_input->setMinimumSize( zoom_in_input->sizeHint() );
  hLayout->addWidget( zoom_in_input, 2 );
  connect( zoom_in_input, SIGNAL( returnPressed() ),
           this, SLOT ( setZoomIn() ) );

  vLayout->addSpacing( 8 );
  vLayout->addStretch( 1 );

  hLayout = new QHBoxLayout();
  vLayout->addLayout( hLayout, 2 );

  text_zoom_out = new QLabel( this );
  CHECK_PTR( text_zoom_out );
  text_zoom_out->setText( i18n("Zoom out factor:") );
  text_zoom_out->setMinimumSize( text_zoom_out->sizeHint() );
  hLayout->addWidget( text_zoom_out, 2 );
  hLayout->addSpacing( 5 );
  hLayout->addStretch( 1 );
  zoom_out_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( zoom_out_input );
  zoom_out_input->setMaxLength( 20 );
  setDblValue( my_zoom_out, zoom_out_input, 10 );
  zoom_out_input->setMinimumSize( zoom_out_input->sizeHint() );
  hLayout->addWidget( zoom_out_input, 2 );
  connect( zoom_out_input, SIGNAL( returnPressed() ),
           this, SLOT ( setZoomOut() ) );

  vLayout->addSpacing( 8 );
  vLayout->addStretch( 1 );

  hLayout = new QHBoxLayout();
  vLayout->addLayout( hLayout, 2 );
 
  ok = new QPushButton( this );
  CHECK_PTR( ok );
  ok->setText( i18n("OK") );
  connect( ok, SIGNAL( clicked() ), SLOT( zoomAccept() ) );
  defaultb = new QPushButton( this );
  CHECK_PTR( defaultb );
  defaultb->setText( i18n("Defaults") );
  connect( defaultb, SIGNAL( clicked() ), SLOT( zoomDefaults() ) );
  cancel = new QPushButton( this );
  CHECK_PTR( cancel );
  cancel->setText( i18n("Cancel") );
  connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );

  QSize buttonSize = ok->sizeHint();
  if ( defaultb->sizeHint().width() > buttonSize.width() )
          buttonSize = defaultb->sizeHint();
  if ( cancel->sizeHint().width() > buttonSize.width() )
          buttonSize = cancel->sizeHint();
  ok->setMinimumSize( buttonSize );
  hLayout->addWidget( ok, 2 );
  hLayout->addSpacing( 5 );
  hLayout->addStretch( 1 );
  defaultb->setMinimumSize( buttonSize );
  hLayout->addWidget( defaultb, 2 );
  hLayout->addSpacing( 5 );
  hLayout->addStretch( 1 );
  cancel->setMinimumSize( buttonSize );
  hLayout->addWidget( cancel, 2 );

  setCaption( i18n("Zoom factors") );

  vLayout->activate();
  }



void ZoomDlg::setZoomIn()
  {
  readDblValue( my_zoom_in, zoom_in_input, 10 );
  }


void ZoomDlg::setZoomOut()
  {
  readDblValue( my_zoom_out, zoom_out_input, 10 );
  }


void ZoomDlg::zoomDefaults()
  {
  my_zoom_in = my_zoom_in_default;
  setDblValue( my_zoom_in, zoom_in_input, 10 );
  my_zoom_out = my_zoom_out_default;
  setDblValue( my_zoom_out, zoom_out_input, 10 );
  }



void ZoomDlg::zoomAccept()
  {
  emit( zoomInChanged( my_zoom_in ) );
  emit( zoomOutChanged( my_zoom_out ) );
  accept();
  }


