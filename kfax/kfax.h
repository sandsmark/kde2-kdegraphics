    /*

    $Id: kfax.h 86135 2001-03-09 17:42:00Z mueller $

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Bernd Johannes Wuebben
                       wuebben@math.cornell.edu


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

#ifndef _KFAX_H_
#define _KFAX_H_

#include <qlist.h>
#include <qstring.h>

#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qkeycode.h>
#include <qaccel.h>
#include <qobject.h>
#include <qmultilineedit.h>
#include <qradiobutton.h>
#include <qfiledialog.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qtooltip.h>
#include <qregexp.h>
#include <qprinter.h>

#include <kapp.h>
#include <kconfig.h>
#include <ktoolbar.h>

#include <kstatusbar.h>
#include <kmainwindow.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/cursorfont.h>

#include "faxexpand.h"

#include "print.h"
#include "options.h"

#define Pimage(p)	((XImage *)(p)->extra)

// StatusBar field IDs
#define ID_LINE_COLUMN 1
#define ID_INS_OVR 2
#define ID_GENERAL 3
#define ID_FNAME 4
#define ID_TYPE 5
#define ID_PAGE_NO 6

// toolbar ids
#define ID_PREV 11
#define ID_PAGE 12
#define ID_NEXT 13

#define ID_PRINT 15
#define ID_MARK 16
#define ID_START 17
#define ID_END 18
#define ID_READ 19
#define ID_ZOOM_OUT 21
#define ID_ZOOM_IN 20

typedef KToolBar::BarPosition BarPosition;

class MyApp:public KApplication {
public:
   virtual bool x11EventFilter( XEvent * );
};

class TopLevel : public KMainWindow
{
    Q_OBJECT


public:

    TopLevel( QWidget *parent=0, const char *name=0 );
    ~TopLevel();

    void openNetFile( const KURL& _url);
    void saveNetFile( const KURL& _url );

    static QList<TopLevel> windowList;
    QPopupMenu *file, *view, *help,*options,*right_mouse_button, *colors;

    void closeEvent( QCloseEvent *e );

    void setFaxTitle(const QString& name);
    void setStatusBarMemField(int mem);
    void handle_X_event(XEvent Event);
    void putImage();
//    void setFaxDefaults();

protected:

    void resizeEvent(QResizeEvent *);
    void dropEvent( QDropEvent * event );

    void setSensitivity();

    void readSettings();
    void writeSettings();
    void setupMenuBar();
    void setupToolBar();
    void setupEditWidget();
    void setupStatusBar();

private:


    int statusID, toolID, indentID;
    KMenuBar *menubar;
    KToolBar *toolbar;
    QColor forecolor;
    QColor backcolor;

    KStatusBar *statusbar;
    QTimer *statusbar_timer;

    int open_mode;
    int editor_width;
    int editor_height;
    int hide_toolbar;
    int hide_statusbar;

    KConfig *config;
    QString current_directory;
    QFileDialog* file_dialog;

    QScrollBar *	hsb;
    QScrollBar *	vsb;
    QFrame* 	mainpane;

    PrintDialog * printdialog;
    struct printinfo pi;

public slots:

    void set_colors();
    void pageActivated(const QString&);
    void saving_slot();
    void loading_slot();
    void setGeneralStatusField(QString string);
    void print();
    void timer_slot();
    void quiteditor();
    void file_open();
    void toolbarClicked( int );
    void zoomin();
    void zoomout();
    void resizeView();
    void file_save();
    void helpselected();
    void helpshort();
    void toggleStatusBar();
    void toggleToolBar();
    void dummy();

    void faxoptions();
    void rotatePage();
    void mirrorPage();
    void flipPage();
    void addFax();
    void closeFax();
    void nextPage();
    void prevPage();
    void newPage();
    void goToEnd();
    void goToStart();
    void helptiff();
    void uiUpdate();

    void openadd(QString filename);
    void printIt();
    void FreeFax();
    void scrollHorz(int);
    void scrollVert(int);
    void toolbarMoved(BarPosition);
};

void kfaxerror(const QString&, const QString&);

#endif
