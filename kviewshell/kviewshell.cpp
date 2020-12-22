#include <qtimer.h>
#include <qdragobject.h>
#include <qregexp.h>

#include <kiconloader.h>
#include <kstddirs.h>
#include <kapp.h>
#include <kaction.h>
#include <kdebug.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <klibloader.h>
#include <kstdaction.h>
#include <khelpmenu.h>
#include <kedittoolbar.h>

#include <stdlib.h>
#include <unistd.h>

#include "kviewpart.h"
#include "kviewshell.moc"


#define StatusBar_ID_PageSize 1

//TODO -- use the statusbar
//TODO -- don't stay open when no file, go directly to KFileDialog


KViewShell::KViewShell(QString partname, KAboutData *about)
  : KParts::MainWindow(), _partname(partname)
{
  // create the viewer part

  // Try to load
  KLibFactory *factory = KLibLoader::self()->factory("libkviewerpart");
  if (factory) {
    QStringList args;
    args << _partname;
    view = (KViewPart*) factory->create(this, "kviewerpart", "KViewPart", args);
  } else {
    KMessageBox::error(this, "No libkviewerpart found !");
    ::exit(-1);
  }

  setCentralWidget(view->widget());

  // file menu
  KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());
  recent = KStdAction::openRecent (this, SLOT(openURL(const KURL &)), actionCollection());
  closeAction = KStdAction::close(this, SLOT(slotFileClose()), actionCollection());
  KStdAction::quit (this, SLOT(slotQuit()), actionCollection());

  // settings menu
  showtoolbar = KStdAction::showToolbar(this, SLOT(slotShowToolBar()), actionCollection());
  showstatusbar = KStdAction::showStatusbar(this, SLOT(slotShowStatusBar()), actionCollection());

  m_helpMenu = new KHelpMenu( this, about, true, actionCollection() );

  setXMLFile( "kviewshell.rc" );

  createGUI(view);

  readSettings();

  checkActions();

  setAcceptDrops(true);

  statusBar()->insertItem("A4",StatusBar_ID_PageSize);
}


void KViewShell::checkActions()
{
  bool doc = !view->url().isEmpty();

  closeAction->setEnabled(doc);
}


KViewShell::~KViewShell()
{
  writeSettings();
  delete view;
}


void KViewShell::slotQuit()
{
  kapp->closeAllWindows();
  kapp->quit();
}


void KViewShell::readSettings()
{
  KConfig *config = kapp->config();

  config->setGroup("General");

  showtoolbar->setChecked(config->readBoolEntry("ShowToolBar", true));
  slotShowToolBar();

  showstatusbar->setChecked (config->readBoolEntry ("ShowStatusBar", true));
  slotShowStatusBar();

  int w = config->readNumEntry ("WindowWidth", 600);
  int h = config->readNumEntry ("WindowHeight", 300);
  resize(w, h);

  recent->loadEntries(config, QString("%1-RecentFiles").arg(_partname));
}


void KViewShell::writeSettings()
{
  KConfig *config = kapp->config();

  config->setGroup( "General" );

  config->writeEntry ("ShowToolBar", showtoolbar->isChecked());
  config->writeEntry ("ShowStatusBar", showstatusbar->isChecked());
  config->writeEntry ("WindowWidth", width());
  config->writeEntry ("WindowHeight", height());

  recent->saveEntries(config, QString("%1-RecentFiles").arg(_partname));

  config->sync();
}


void KViewShell::openURL(const KURL &url)
{
  if (view->openURL(url))
    {
      recent->addURL(url);
      checkActions();
    }
}


void KViewShell::slotFileOpen()
{
  KURL url = KFileDialog::getOpenURL(QString::null, view->fileFormats().join("\n"));

  if (!url.isEmpty())
    openURL(url);
}


void KViewShell::slotFileClose()
{
  view->closeURL();

  checkActions();
}


void KViewShell::slotShowStatusBar()
{
  if (showstatusbar->isChecked())
    statusBar()->show();
  else
    statusBar()->hide();
}


void KViewShell::slotShowToolBar()
{
  if (showtoolbar->isChecked())
    toolBar()->show();
  else
    toolBar()->hide();
}


void KViewShell::slotEditToolbar()
{
  KEditToolbar dlg(actionCollection());
  if (dlg.exec())
    createGUI(view);
}


void KViewShell::dragEnterEvent(QDragEnterEvent *event)
{
  if (QUriDrag::canDecode(event))
    {
      QStringList urls;
      QUriDrag::decodeToUnicodeUris(event, urls);
      if (urls.count() > 0)
	{
	  // look if the extension is right
	  QStringList list = view->fileFormats();
	  QStringList::Iterator it;
	  for (it = list.begin(); it != list.end(); ++it)
	    {
	      QString ext = *it;
	      int pos = ext.find('|');
	      if (pos >= 0)
		ext = ext.left(pos);

	      QRegExp re(ext, false, true);
	      if (re.match(urls[0]) >= 0)
		{
		  event->accept(true);
		  return;
		}
	    }

	}
    }
  
  event->accept(false);
}


void KViewShell::dropEvent(QDropEvent *event)
{
  if (QUriDrag::canDecode(event))
    {
      QStringList urls;
      QUriDrag::decodeToUnicodeUris(event, urls);
      if (urls.count() > 0)
	openURL(KURL(urls[0]));
    }
}


