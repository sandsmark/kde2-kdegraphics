/*
* kview.cpp -- Implementation of class KView.
* Author:  Sirtaj Singh Kang
* Version:  $Id: kview.cpp 98288 2001-05-22 15:42:52Z mkretz $
* Generated:  Wed Oct 15 01:26:27 EST 1997
*/

#include <assert.h>

#include <qimage.h>
#include <qfile.h>

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kimageio.h>

#include "kview.h"
#include "viewer.h"
#include "filter.h"
#include "filtlist.h"
#include "kfilteraction.h"
#include "colour.h"

KView::KView()
  : QObject( 0 ),
  _app(),
  _filters( new KFilterList ),
  _viewers( new QList<KImageViewer> ),
  _cutBuffer( 0 )
{
  assert( _filters );

  _viewers->setAutoDelete( true );

  KImageIO::registerFormats();

  registerBuiltinFilters();
}

KView::~KView()
{
  delete _viewers; _viewers = 0;
  delete _filters; _filters = 0;
}

int KView::exec()
{
   if( _app.isRestored() ) 
   {
      // restore saved viewers
      for ( int i = 1; KImageViewer::canBeRestored( i ); i++ ) 
      {
         makeViewer()->restore( i );
      }
   }
   else 
   {
      KImageViewer *viewer = makeViewer();

      // process arguments only if not restored
      KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
      for( int i = 0; i < args->count(); i++ ) 
      {
         viewer->slot_appendURL( args->url( i ), (i == 0) );
      }
      viewer->show();
      viewer->slot_firstImage();
   }

   return _app.exec();
}

void KView::registerBuiltinFilters()
{
  _filters->registerFilter( new BriteFilter, KFilterList::AutoDelete );
  _filters->registerFilter( new GreyFilter, KFilterList::AutoDelete );
  _filters->registerFilter( new SmoothFilter, KFilterList::AutoDelete );
  _filters->registerFilter( new GammaFilter, KFilterList::AutoDelete );
}

void KView::newViewer()
{
  KImageViewer *viewer = makeViewer();
  
  viewer->show();
}

KImageViewer *KView::makeViewer()
{
  KImageViewer *viewer = new KImageViewer( _filters );

  if( viewer == 0 )
    return 0;

  _viewers->append( viewer );

  connect( viewer, SIGNAL( wantNewViewer() ),
    this, SLOT( newViewer() ) );
  connect( viewer, SIGNAL( wantToDie( KImageViewer* ) ),
    this, SLOT( closeViewer( KImageViewer* ) ) );

  return viewer;
}

void KView::closeViewer( KImageViewer *viewer )
{
  assert( viewer != 0 );

  _viewers->remove( viewer );

  if( _viewers->count() == 0 ) 
  {
    kapp->quit();
  }
}

void KView::setCutBuffer( QPixmap * )
{
  if( _cutBuffer )
  {
    delete _cutBuffer; _cutBuffer = 0;
  }
}

#include "kview.moc"

