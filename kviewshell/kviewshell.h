#ifndef __KVIEWSHELL_H__
#define __KVIEWSHELL_H__


#include <qstring.h>
#include <qevent.h>


#include <kparts/mainwindow.h>
#include <kurl.h>


class KRecentFilesAction;
class ScrollBox;
class KToggleAction;
class KViewPart;
class KHelpMenu;
class KAboutData;


class KViewShell : public KParts::MainWindow
{
  Q_OBJECT

public:

  KViewShell(QString partname, KAboutData *);
  virtual ~KViewShell();

public slots:

  void openURL(const KURL &url);


protected slots:

  void slotFileOpen();
  void slotShowToolBar();
  void slotShowStatusBar();
  void slotQuit();
  void slotEditToolbar();
  void slotFileClose();


protected:

  void readSettings();
  void writeSettings();

  void checkActions();

  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);


private:

  QString _partname;
  KViewPart *view;

  KRecentFilesAction *recent;
  QString cwd;

  KAction *closeAction;
  KToggleAction *showtoolbar, *showstatusbar;

  KHelpMenu *m_helpMenu;
};


#endif
