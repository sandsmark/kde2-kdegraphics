#include <qobject.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qscrollview.h>


#include <kaction.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <kio/job.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstdaction.h>


#include "marklist.h"
#include "scrollbox.h"

#include "kmultipage.moc"


KMultiPage::KMultiPage(QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name)
  : KParts::ReadOnlyPart(parent, name), mainWidget(0)
{
  _scrollView = new CenteringScrollview(parentWidget, widgetName);
  _scrollView->setResizePolicy(QScrollView::AutoOne);
  _scrollView->setVScrollBarMode(QScrollView::Auto);
  _scrollView->setHScrollBarMode(QScrollView::Auto);
  _scrollView->viewport()->setBackgroundMode(QWidget::PaletteMid );

  setWidget(_scrollView);
}


KMultiPage::~KMultiPage()
{
}


void KMultiPage::slotSave()
{
  // Try to guess the proper ending...
  QString formats;
  QString ending;
  int rindex = m_file.findRev(".");
  if ( rindex == -1) {
    ending = QString::null;
    formats = QString::null;
  } else {
    ending = m_file.mid(rindex); // e.g. ".dvi"
    formats = fileFormats().grep(ending).join("\n");
  }

  QString fileName = KFileDialog::getSaveFileName(QString::null, formats, 0, i18n("Save File As"));

  if (fileName.isEmpty())
    return;

  // Add the ending to the filename. I hope the user likes it that
  // way.
  if (!ending.isEmpty() && fileName.find(ending) == -1)
    fileName = fileName+ending;
  
  if (QFile(fileName).exists()) {
    int r = KMessageBox::warningYesNo (0, QString(i18n("The file %1\nexists. Shall I overwrite that file?")).arg(fileName), 
				       i18n("Overwrite file"));
    if (r == KMessageBox::No)
      return;
  }

  KIO::Job *job = KIO::file_copy( m_file, fileName, 0600, true, false, true );
  connect( job, SIGNAL( result( KIO::Job * ) ), this, SLOT( slotIOJobFinished ( KIO::Job * ) ) );

  return;
}

void KMultiPage::slotIOJobFinished ( KIO::Job *job )
{
  if ( job->error() )
    job->showErrorDialog( 0L );
}
