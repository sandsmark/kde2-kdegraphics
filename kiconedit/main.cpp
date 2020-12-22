/*
    KDE Draw - a small graphics drawing program for the KDE.
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)
 
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

// A lot of this code is lifted from KMail. Thanks, guys!

#include <signal.h>
#include <stdlib.h>


#include <kapp.h>
#include <klocale.h>
#include <kconfig.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kdebug.h>

#include "main.h"
#include "version.h"
#include "kiconedit.h"
#include "config.h"

static const char *description = 
	I18N_NOOP("KDE Icon Editor");

static KCmdLineOptions options[] =
{
  { "+[file(s)]" , I18N_NOOP("Icon file(s) to open"), 0 },
  { 0, 0, 0 }
};


WindowList* windowList = NULL;

static void signalHandler(int sigId);
static void cleanup(void);
static void setSignalHandler(void (*handler)(int));

KSWApplication::KSWApplication ( )  
{
  _ksw_save = XInternAtom(qt_xdisplay(),"KSW_SAVE",False);
  _ksw_exit = XInternAtom(qt_xdisplay(),"KSW_EXIT",False);
}

bool KSWApplication :: x11EventFilter( XEvent *ev)
{
  XClientMessageEvent* e = &ev->xclient;

  if (e->message_type == _ksw_save)
  {
    kdDebug() << "Got KSW_SAVE" << endl;
    emit ksw_save();
    return true;
  }
  else if (e->message_type == _ksw_exit)
  {
    kdDebug() << "Got KSW_EXIT" << endl;
    emit ksw_exit();
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
// Crash recovery signal handler
static void signalHandler(int sigId)
{
  QWidget* win;

  if(!windowList)
    exit(1);

  kdDebug() << "*** KIconEdit got signal " << sigId << endl;

  // try to cleanup all windows
  while (windowList->first() != NULL)
  {
    win = windowList->take();
    // if (win->inherits("KIconEdit")) ((KIconEdit*)win)->saveGoingDownStatus();
    delete win;
  }

  setSignalHandler(SIG_DFL);
  cleanup();
  exit(1);
}


//-----------------------------------------------------------------------------
static void setSignalHandler(void (*handler)(int))
{
  signal(SIGSEGV, handler);
  signal(SIGKILL, handler);
  signal(SIGTERM, handler);
  signal(SIGHUP,  handler);
  signal(SIGFPE,  handler);
  signal(SIGABRT, handler);
}

static void cleanup(void)
{
  kapp->config()->sync();
}

int main(int argc, char **argv)
{
  KAboutData aboutData( "kiconedit", I18N_NOOP("KIconEdit"),
    VERSION, description, KAboutData::License_GPL,
    "(c) 1998, Thomas Tanghus");
    
  aboutData.addAuthor("Thomas Tanghus",0, "tanghus@kde.org");
  aboutData.addAuthor("John Califf",0, "jcaliff@compuzone.net");
  aboutData.addAuthor("Laurent Montel",0, "lmontel@mandrakesoft.com");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
  
  KSWApplication a;

  setSignalHandler(signalHandler);

  if (a.isRestored())
  {
    RESTORE(KIconEdit);
  }
  else
  {
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    for(int i = 0; i < args->count(); i++)
    {
        KIconEdit *ki = new KIconEdit(QFile::decodeName(args->arg(i)), "kiconedit");
        CHECK_PTR(ki);
    }

    if (args->count() == 0)
    {
        KIconEdit *ki = new KIconEdit;
        CHECK_PTR(ki);
    }
    args->clear();
  }

  return a.exec();
}
#include "main.moc"
