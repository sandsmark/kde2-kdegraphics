/*  This file is part of the KDE project
    Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>

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

#include "kview_view.h"
#include "canvas.h"
//#include "knumdialog.h"
#include "kview_asyncio.h"

#include <qiconset.h>
#include <qimage.h>
#include <qdragobject.h>
#include <qevent.h>
#include <kprinter.h>

#include <kaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <kio/job.h>
#include <kimageio.h>
#include <kinstance.h>
#include <kfileitem.h>
#include <kxmlgui.h>
#include <kfiledialog.h>
#include <ktempfile.h>

extern "C"
{
  void *init_libkviewpart()
  {
    KImageIO::registerFormats();
    return new KViewFactory;
  }
};

KInstance *KViewFactory::s_instance = 0L;

KViewFactory::KViewFactory()
{
}

KViewFactory::~KViewFactory()
{
  if ( s_instance )
    delete s_instance;

  s_instance = 0;
}

KParts::Part *KViewFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *, const QStringList & )
{
  KParts::Part *obj = new KViewPart( parentWidget, widgetName, parent, name );
  emit objectCreated( obj );
  return obj;
}

KInstance *KViewFactory::instance()
{
  if ( !s_instance )
    s_instance = new KInstance( "kview" );
  return s_instance;
}

void KViewKonqExtension::print()
{
  KPrinter printer;

  if ( !printer.setup( ((KViewPart *)parent())->widget() ) )
    return;

  QPainter painter;
  painter.begin( &printer );

  for ( int i = 0; i < printer.numCopies(); i++ )
  {
    painter.drawPixmap( 0, 0, *(canvas()->client()->imagePix()) );
    if ( i < printer.numCopies() - 1 )
      printer.newPage();
  }

  painter.end();
}

KViewPart::KViewPart( QWidget *parentWidget, const char * /*widgetName*/, QObject *parent, const char *name )
 : KParts::ReadOnlyPart( parent, name )
 , m_job(0)
 , m_TempFile(0)
{
  setInstance( KViewFactory::instance() );

  m_extension = new KViewKonqExtension( this );

  zoomFactor = 0;
  m_pCanvas = new KImageCanvas( parentWidget ); // ### obey widgetName (Simon)
  m_pCanvas->setFocusPolicy( QWidget::StrongFocus );
  setWidget( m_pCanvas );

  KVImageHolder *img = m_pCanvas->client();
  img->move( 0, 0 );
  img->show();
  m_pCanvas->show();

  m_paZoomIn = new KAction( i18n( "Zoom in 10%" ), "viewmag+", 0, this, SLOT( slotZoomIn() ), actionCollection(), "zoomin" );
  m_paZoomOut = new KAction( i18n( "Zoom out 10%" ), "viewmag-", 0, this, SLOT( slotZoomOut() ), actionCollection(), "zoomout" );
  m_paRotate = new KAction( i18n( "Rotate counter-clockwise" ), "rotate", 0, this, SLOT( slotRotate() ), actionCollection(), "rotate" );
  m_paReset = new KAction( i18n( "Reset" ), "undo", 0, this, SLOT( slotReset() ), actionCollection(), "reset" );
    (void) new KAction( i18n( "Save Image As ..." ), 0, this, SLOT( slotSaveImageAs() ),
                        actionCollection(), "saveimageas" );

  setXMLFile( "kview_part.rc" );

  connect( m_pCanvas, SIGNAL( contextPress( const QPoint & ) ),
           this, SLOT( slotPopupMenu( const QPoint & ) ) );

  m_popupDoc = KXMLGUIFactory::readConfigFile( "kview_popup.rc", true, instance() );
}

KViewPart::~KViewPart()
{
  closeURL();
}

bool KViewPart::openURL( const KURL &url )
{
  closeURL();

  // write to a tempfile, so save as will work
  m_TempFile = new KTempFile;
  m_TempFile->setAutoDelete(true);

  m_job = KIO::get( url, m_extension->urlArgs().reload, false );

  m_mimeType = m_extension->urlArgs().serviceType;

  //m_job->setGUImode( KIO::Job::NONE );

  QObject::connect( m_job, SIGNAL( result( KIO::Job * ) ), this, SLOT( slotResult( KIO::Job * ) ) );
  //QObject::connect( job, SIGNAL( sigRedirection( int, const char * ) ), this, SLOT( slotRedirection( int, const char * ) ) );

  (void)new KIOImageLoader( m_job, m_pCanvas->client(), m_TempFile );
  m_pCanvas->forgetOriginal();
  matrix.reset();

  m_url = url;

  emit started( m_job );
  emit setWindowCaption( m_url.prettyURL() );

  return true;
}

bool KViewPart::closeURL()
{
  if ( m_job )
  {
    m_job->kill();
    m_job = 0;
  }
  if (m_TempFile)
  {
    delete m_TempFile;
    m_TempFile = 0;
  }
  return true;
}

void KViewPart::slotZoomIn()
{
  matrix.scale( 1.1, 1.1 );
  m_pCanvas->transformImage( matrix );
}

void KViewPart::slotZoomOut()
{
  matrix.scale( 0.9, 0.9 );
  m_pCanvas->transformImage( matrix );
}

void KViewPart::slotRotate()
{
  matrix.rotate( -90 );
  m_pCanvas->transformImage( matrix );
}

void KViewPart::slotReset()
{
  matrix.reset();
  m_pCanvas->transformImage( matrix );
}

void KViewPart::keyPressEvent( QKeyEvent *ev )
{
  switch ( ev->key() )
  {
    case Key_Down:
      ev->accept();
      m_pCanvas->slot_lineDown();
      break;
    case Key_Up:
      ev->accept();
      m_pCanvas->slot_lineUp();
      break;
    case Key_Left:
      ev->accept();
      m_pCanvas->slot_lineLeft();
      break;
    case Key_Right:
      ev->accept();
      m_pCanvas->slot_lineRight();
      break;
    case Key_PageUp:
      ev->accept();
      m_pCanvas->slot_pageUp();
      break;
    case Key_PageDown:
      ev->accept();
      m_pCanvas->slot_pageDown();
      break;
    default:
      ev->ignore();
      break;
  }
}

class PopupGUIClient : public KXMLGUIClient
{
public:
    PopupGUIClient( KInstance *inst, const QString &doc )
        {
            setInstance( inst );
            setXML( doc );
        }
};

void KViewPart::slotPopupMenu( const QPoint &pos )
{
    KXMLGUIClient *popupGUIClient = new PopupGUIClient( instance(), m_popupDoc );

    (void) new KAction( i18n( "Save Image As ..." ), 0, this, SLOT( slotSaveImageAs() ),
                        popupGUIClient->actionCollection(), "saveimageas" );

    // ### HACK treat the image as dir to get the back/fwd/reload buttons (Simon)
    emit m_extension->popupMenu( popupGUIClient, pos, m_url, m_mimeType, S_IFDIR );

    delete popupGUIClient;
}

void KViewPart::slotSaveImageAs()
{
    KFileDialog *dlg = new KFileDialog( QString::null, QString::null, widget(), "filedia", true );

    dlg->setKeepLocation( true );

    dlg->setCaption( i18n( "Save Image As" ) );

    if ( !m_url.fileName().isEmpty() )
        dlg->setSelection( m_url.fileName() );

    if ( dlg->exec() )
    {
        KURL destURL( dlg->selectedURL() );
        if ( !destURL.isMalformed() )
        {
            // use our tempfile
            KURL url;
            url.setPath(m_TempFile->name());

            KIO::Job *job = KIO::copy( url, destURL );
            connect( job, SIGNAL( result( KIO::Job * ) ),
                     this, SLOT( slotResultSaveImageAs( KIO::Job * ) ) );
        }
    }

    delete dlg;
}

void KViewPart::slotResultSaveImageAs( KIO::Job *job )
{
    if ( job->error() )
        job->showErrorDialog( widget() );
}

void KViewPart::slotResult( KIO::Job * job )
{
  if (job->error())
  {
    // error dialog already shown by KIOImageLoader
    emit canceled( job->errorString() );
  } else
  {
    m_pCanvas->updateScrollBars();
    emit completed();
  }
  m_job = 0;
}

/*
void KViewPart::slotRedirection( int, const QString & url )
{
  QString sUrl ( url );
  m_url = KURL( sUrl );
  emit m_extension->setLocationBarURL( sUrl );
  emit setWindowCaption( m_url.prettyURL() );
}
*/

KViewKonqExtension::KViewKonqExtension( KViewPart *parent,
                                        const char *name )
: KParts::BrowserExtension( parent, name )
{
}

void KViewKonqExtension::setXYOffset( int x, int y )
{
  canvas()->updateScrollBars();
  canvas()->setContentsPos( x, y );
}

int KViewKonqExtension::xOffset()
{
  return canvas()->contentsX();
}

int KViewKonqExtension::yOffset()
{
  return canvas()->contentsY();
}

/*
void KViewKonqView::resizeEvent( QResizeEvent * )
{
  m_pCanvas->setGeometry( 0, 0, width(), height() );
}
*/

#include "kview_view.moc"


