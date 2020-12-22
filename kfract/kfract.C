
    /*

    kfract.C  version 0.1.2

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

#include <iostream.h>
#include <qkeycode.h>

#include "kfract.moc"
#include "version.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kmenubar.h>

#include <kaction.h>
#include <kstdaction.h>


FractMainWindow::FractMainWindow() :
        KMainWindow( 0, "kfract" )
{
}

bool FractMainWindow::queryClose()
{
  emit aboutToClose();
  return true;
}


KFract::KFract( ) 
{
  center = width = height = 0.0;
  w = new FractMainWindow();
  CHECK_PTR( w );
  setMainWidget( w );
  connect( w, SIGNAL( aboutToClose() ),
           this, SLOT( willClose() ) );

  v = new DrawView( w );
  CHECK_PTR( v );

  KAction *action;

  // keyboard accelerators
  accel = new KAccel(w);
  accel->insertItem(i18n("Mandelbrot Set"), "options_mandel", Qt::CTRL+Qt::Key_M);
  accel->insertItem(i18n("Julia Set"), "options_julia", Qt::CTRL+Qt::Key_J);
  accel->insertItem(i18n("Save Pic"), "file_save_pic", Qt::CTRL+Qt::Key_P);
  accel->readSettings();

  // file menu
  action = KStdAction::open(this, SLOT(loadParam()), w->actionCollection());
  action->setText(i18n("&Open params..."));
  action = KStdAction::saveAs(this, SLOT(saveParam()), w->actionCollection());
  action->setText(i18n("&Save params..."));
  action = new KAction(i18n("Save &pic..."), "filesave",
                       accel->currentKey("file_save_pic"), this,
                       SLOT(savePic()), w->actionCollection(),
                       "file_save_pic");
  action = KStdAction::quit(this, SLOT(my_quit()), w->actionCollection());

  // view menu
  action = KStdAction::zoomIn(this, SLOT(doZoomIn()), w->actionCollection());
  action = KStdAction::zoomOut(this, SLOT(doZoomOut()), w->actionCollection());
  action = KStdAction::zoom(this, SLOT(zoomFract()), w->actionCollection());

  // options menu
  mandel = new KAction(i18n("&Mandelbrot..."), "kfract",
		       accel->currentKey("options_mandel"), this,
		       SLOT(mandelFract()), w->actionCollection(),
		       "options_mandel");
  julia = new KAction(i18n("&Julia..."), "kfract",
		       accel->currentKey("options_julia"), this,
		       SLOT(juliaFract()), w->actionCollection(),
		       "options_julia");
  action = new KAction(i18n("&Iterations..."), 0, this,
		       SLOT(iterFract()), w->actionCollection(),
		       "options_iter");
//  action = new KAction(i18n("Color &scheme..."), 0, this,
//		       SLOT(colorSchemeFract()), w->actionCollection(),
//		       "options_scheme");
  action = KStdAction::keyBindings(this, SLOT(keyBindings()), w->actionCollection());

  w->createGUI("kfract.rc");

  w->setCentralWidget( v );
  w->show();
  w->resize( KFRACT_INITIAL_SIZE_X + KFRACT_SIZE_DIFF_X, 
             KFRACT_INITIAL_SIZE_Y + KFRACT_SIZE_DIFF_Y );
};


void KFract::keyBindings()
{
  if (KKeyDialog::configureKeys(accel))
    {
      mandel->setAccel(accel->currentKey("options_mandel"));
      julia->setAccel(accel->currentKey("options_julia"));
    }
}


void KFract::mandelFract()
  {
  v->setNewType( DrawView::FMandel );
  MandelDlg dlg( v->getActualIter(),

                 v->getDefaultIter(),
                 v->getActualBailout(),
                 v->getDefaultBailout(),
                 v->getActualCenterX(),
                 v->getDefaultCenterX(),
                 v->getActualCenterY(),
                 v->getDefaultCenterY(),
                 v->getActualWidth(),
                 v->getDefaultWidth(),
                 w );
  connect( &dlg, SIGNAL( iterChanged( int ) ),
           this, SLOT( getIter( int ) ) );
  connect( &dlg, SIGNAL( bailoutChanged( double ) ),
           this, SLOT( getBailout( double ) ) );
  connect( &dlg, SIGNAL( centerXChanged( double ) ),
           this, SLOT( getCenterX( double ) ) );
  connect( &dlg, SIGNAL( centerYChanged( double ) ),
           this, SLOT( getCenterY( double ) ) );
  connect( &dlg, SIGNAL( widthChanged( double ) ),
           this, SLOT( getWidth( double ) ) );
  connect( &dlg, SIGNAL( changedToMandel() ),
           this, SLOT( changedToMandel() ) );
  dlg.exec();
  v->setTypeUndo();
  }


void KFract::juliaFract()
  {
  v->setNewType( DrawView::FJulia );
  JuliaDlg dlg( v->getActualIter(),
                v->getDefaultIter(),
                v->getActualBailout(),
                v->getDefaultBailout(),
                v->getActualCenterX(),
                v->getDefaultCenterX(),
                v->getActualCenterY(),
                v->getDefaultCenterY(),
                v->getActualWidth(),
                v->getDefaultWidth(),
                v->getActualExtraX(),
                v->getDefaultExtraX(),
                v->getActualExtraY(),
                v->getDefaultExtraY(),
                w );
  connect( &dlg, SIGNAL( iterChanged( int ) ),
           this, SLOT( getIter( int ) ) );
  connect( &dlg, SIGNAL( bailoutChanged( double ) ),
           this, SLOT( getBailout( double ) ) );
  connect( &dlg, SIGNAL( centerXChanged( double ) ),
           this, SLOT( getCenterX( double ) ) );
  connect( &dlg, SIGNAL( centerYChanged( double ) ),
           this, SLOT( getCenterY( double ) ) );
  connect( &dlg, SIGNAL( widthChanged( double ) ),
           this, SLOT( getWidth( double ) ) );
  connect( &dlg, SIGNAL( extraXChanged( double ) ),
           this, SLOT( getExtraX( double ) ) );
  connect( &dlg, SIGNAL( extraYChanged( double ) ),
           this, SLOT( getExtraY( double ) ) );
  connect( &dlg, SIGNAL( changedToJulia() ),
           this, SLOT( changedToJulia() ) );
  dlg.exec();
  v->setTypeUndo();
  }



void KFract::iterFract()
  {
  IterDlg dlg( v->getActualIter(),
               v->getDefaultIter(), w );
  connect( &dlg, SIGNAL( iterChanged( int ) ),
           this, SLOT( getIter( int ) ) );
  dlg.exec();
  }


void KFract::zoomFract()
  {
  ZoomDlg dlg( v->getActualZoomInFactor(),
               v->getDefaultZoomInFactor(),
               v->getActualZoomOutFactor(),
               v->getDefaultZoomOutFactor(), w );
  connect( &dlg, SIGNAL( zoomInChanged( double ) ),
           this, SLOT( getZoomIn( double ) ) );
  connect( &dlg, SIGNAL( zoomOutChanged( double ) ),
           this, SLOT( getZoomOut( double ) ) );
  dlg.exec();
  }


void KFract::getZoomIn( double zoom_in )
  {
  v->setNewZoomInFactor( zoom_in );
  }


void KFract::getZoomOut( double zoom_out )
  {
  v->setNewZoomOutFactor( zoom_out );
  }


void KFract::colorSchemeFract()
  {
  // doesn't exist nay more
  }


void KFract::getColorScheme( DrawView::ColorScheme scheme )
  {
  v->setNewColorScheme( scheme );
  }


void KFract::notImplemented()
  {
    KMessageBox::sorry( w, i18n("Oops! Not implemented yet."));
  }


void KFract::helpFract()
  {
  kapp->invokeHelp( "", "" );
  }


void KFract::getIter( int new_iter )
  {
  v->setNewIter( new_iter );
  }


void KFract::getBailout( double new_bailout )
  {
  v->setNewBailout( new_bailout );
  }


void KFract::getCenterX( double new_center_x )
  {
  v->setNewCenterX( new_center_x );
  }


void KFract::getCenterY( double new_center_y )
  {
  v->setNewCenterY( new_center_y );
  }


void KFract::getWidth( double new_width )
  {
  v->setNewWidth( new_width );
  }


void KFract::getExtraX( double new_extra_x )
  {
  v->setNewExtraX( new_extra_x );
  }


void KFract::getExtraY( double new_extra_y )
  {
  v->setNewExtraY( new_extra_y );
  }


void KFract::my_quit()
  {
  v->stop();
  kapp->quit();
  }


void KFract::willClose()
  {
  v->prepareClose();
  }


void KFract::changedToMandel()
  {
  v->changeToMandel();
  }


void KFract::changedToJulia()
  {
  v->changeToJulia();
  }                 


void KFract::savePic()
  {
  v->savePic();
  }


void KFract::saveParam()
  {
  v->saveParam();
  }


void KFract::loadParam()
  {
  v->loadParam();
  }


void KFract::doZoomIn()
  {
  v->doZoomIn();
  }


void KFract::doZoomOut()
  {
  v->doZoomOut();
  }


void KFract::goHome()
  {
  v->goDefaults();
  }


