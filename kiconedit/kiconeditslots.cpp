/*
    KDE Icon Editor - a small graphics drawing program for the KDE
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#include <qdragobject.h>
#include <qstrlist.h>
#include <qprintdialog.h>
#include <qpaintdevicemetrics.h>

#include <klocale.h>
#include <ktoolbarbutton.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "kiconedit.h"

#ifndef PICS_INCLUDED
#define PICS_INCLUDED
#include "pics/logo.xpm"
#endif

void KIconEdit::addColors(uint n, uint *c)
{
    //kdDebug() << "KIconTools::addColors()" << endl;

    customcolors->clear();
    for(uint i = 0; i < n; i++)
        addColor(c[i]);
}

void KIconEdit::addColor(uint color)
{
    //kdDebug() << "KIconTools::addColor()" << endl;

    if(!syscolors->contains(color))
        customcolors->addColor(color);
}

void KIconEdit::slotNewWin()
{
    slotNewWin((const char*)0);
}

void KIconEdit::slotNewWin(const QString & url)
{
    kdDebug() << "KIconEdit::openNewWin() - " << url << endl;
  
    KIconEdit *w = new KIconEdit(url, "kiconedit");
    CHECK_PTR(w);
}


void KIconEdit::slotNew()
{
    bool cancel = false;
    if (grid->isModified()) 
    {
        int r = KMessageBox::warningYesNoCancel(this, 
        i18n("The current file has been modified.\nDo you want to save it?"));

        switch(r)
        {
            case 0:
                icon->save(&grid->image());
                break;

            case 1:
                break;

            case 2:
                cancel = true;
                break;

            default:
                break;
        }
    }
    if(!cancel)
    {
        KNewIcon newicon(this);
        if(newicon.exec())
        {
            int r = newicon.openStyle();
            if(r == KNewIcon::Blank)
            {
                grid->editClear();
                const QSize s = newicon.templateSize();
                kdDebug() << "Size: " << s.width() << " x " << s.height() << endl;
                grid->setSize(s);
                grid->setModified(false);
            }
            else if(r == KNewIcon::Template)
            {
                QString str = newicon.templatePath();
                icon->open(&grid->image(), str);
            }
        }
    }
}


void KIconEdit::slotOpen()
{
    bool cancel = false;
  
    if( grid->isModified() ) 
    {
        int r = KMessageBox::warningYesNoCancel(this, 
      i18n("The current file has been modified.\nDo you want to save it?"));
        
        switch( r )
        {
            case 0:
                icon->save( &grid->image() );
                break;

            case 1:
                break;

            case 2:
                cancel = true;
                break;

            default:
                break;
        }
    }
    
    if( !cancel )
        icon->promptForFile( &grid->image() );
}

/*
    close only the current window
*/
void KIconEdit::slotClose()
{
    close();
}

/*
    close all member windows
*/
void KIconEdit::slotQuit()
{
    kdDebug() << "KIconEdit: Closing " <<  endl;
    
    KIconEdit *ki = 0L;
    while((ki = (KIconEdit*)memberList->getFirst()) != 0)
    {
        CHECK_PTR(ki);
        memberList->getFirst()->close();
    }
}


void KIconEdit::slotSave()
{
    kdDebug() << "KIconEdit: slotSave() " << endl;
    icon->save(&grid->image());
}


void KIconEdit::slotSaveAs()
{
    kdDebug() << "KIconEdit: slotSaveAs() " << endl;
    icon->saveAs(&grid->image());
}


void KIconEdit::slotPrint()
{
    file->setItemEnabled(ID_FILE_PRINT, false);
    toolbar->setItemEnabled(ID_FILE_PRINT, false);

    if ( printer->setup(this) )
    {
        int margin = 10, yPos = 0;
        printer->setCreator("KDE Icon Editor");
    
        QPainter p;
        p.begin( printer );
        QFontMetrics fm = p.fontMetrics();
        // need width/height
        QPaintDeviceMetrics metrics( printer ); 

        p.drawText( margin, margin + yPos, metrics.width(), fm.lineSpacing(),
                        ExpandTabs | DontClip, icon->url() );
        yPos = yPos + fm.lineSpacing();
        p.drawPixmap( margin, margin + yPos, grid->pixmap() );
        p.end();
  }
  
  file->setItemEnabled(ID_FILE_PRINT, true);
  toolbar->setItemEnabled(ID_FILE_PRINT, true);
}


void KIconEdit::slotView( int id )
{
    switch(id)
    {
        case ID_VIEW_ZOOM_1TO1:
            //slotUpdateStatusScaling(1, false);
            grid->zoomTo(1);
            //slotUpdateStatusScaling(1, true);
            toolbar->setItemEnabled(ID_VIEW_ZOOM_OUT, false);
            view->setItemEnabled(ID_VIEW_ZOOM_OUT, false);
            break;

        case ID_VIEW_ZOOM_1TO5:
            //slotUpdateStatusScaling(5, false);
            grid->zoomTo(5);
            toolbar->setItemEnabled(ID_VIEW_ZOOM_OUT, true);
            view->setItemEnabled(ID_VIEW_ZOOM_OUT, true);
            //slotUpdateStatusScaling(5, true);
            break;

        case ID_VIEW_ZOOM_1TO10:
            //slotUpdateStatusScaling(10, false);
            grid->zoomTo(10);
            toolbar->setItemEnabled(ID_VIEW_ZOOM_OUT, true);
            view->setItemEnabled(ID_VIEW_ZOOM_OUT, true);
            //slotUpdateStatusScaling(10, true);
            break;

        case ID_VIEW_ZOOM_OUT:
            if(!grid->zoom(Out))
            {
                toolbar->setItemEnabled(ID_VIEW_ZOOM_OUT, false);
                view->setItemEnabled(ID_VIEW_ZOOM_OUT, false);
            }
            break;

        case ID_VIEW_ZOOM_IN:
            grid->zoom(In);
            toolbar->setItemEnabled(ID_VIEW_ZOOM_OUT, true);
            view->setItemEnabled(ID_VIEW_ZOOM_OUT, true);
            break;

        default:
            break;
    }
}

void KIconEdit::slotCopy()
{
    grid->editCopy();
}

void KIconEdit::slotCut()
{
    grid->editCopy(true);
}

void KIconEdit::slotPaste()
{
    toggleTool(ID_DRAW_FIND);
    grid->setTool(KIconEditGrid::Find);
    grid->editPaste();
}

void KIconEdit::slotClear()
{
    grid->editClear();
}

void KIconEdit::slotSelectAll()
{
    grid->setTool(KIconEditGrid::SelectRect);
    grid->editSelectAll();
}

void KIconEdit::slotOpenRecent(int id)
{
    kdDebug() << "Opening recent file: menu id is: " << id << endl;

    int indx = recent->indexOf(id);
    kdDebug() << "Opening recent file: index of menu id is: " << indx << endl;

    QString fname = recent->text(id);
    kdDebug() << "Opening recent file: string at menu id is: " << fname << endl;
    
    if(!fname.isNull())
    {
        icon->open(&grid->image(), fname);    
    }
}


void KIconEdit::slotConfigure(int id)
{
    switch(id)
    {
        case ID_OPTIONS_CONFIGURE:
        {
            KIconConfig c(this);
            if(c.exec())
            {
                Properties *pprops = props(this);
                updateMenuAccel();
                gridview->setShowRulers(pprops->showrulers);
                if(pprops->backgroundmode == QWidget::FixedPixmap)
                {
                    QPixmap pix(pprops->backgroundpixmap);
                    if(pix.isNull())
                    {
                        QPixmap pmlogo((const char**)logo);
                        pix = pmlogo;
                    }
                    gridview->viewPortWidget()->viewport()->setBackgroundPixmap(pix);
                }
                else
                    gridview->viewPortWidget()->viewport()->setBackgroundColor(pprops->backgroundcolor);
            }
            break;
        }
        
        case ID_OPTIONS_TOGGLE_GRID:
            if(options->isItemChecked(ID_OPTIONS_TOGGLE_GRID))
            {
                options->setItemChecked(ID_OPTIONS_TOGGLE_GRID, false);
                ((KToolBarButton*)toolbar->getButton(ID_OPTIONS_TOGGLE_GRID))->on(false);
                props(this)->showgrid = false;
                grid->setGrid(false);
            }
            else
            {
                options->setItemChecked(ID_OPTIONS_TOGGLE_GRID, true);
                ((KToolBarButton*)toolbar->getButton(ID_OPTIONS_TOGGLE_GRID))->on(true);
                props(this)->showgrid = true;
                grid->setGrid(true);
            }
            break;
            
        case ID_OPTIONS_TOGGLE_TOOL1:
            if(options->isItemChecked(ID_OPTIONS_TOGGLE_TOOL1))
            {
                options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL1, false);
                toolBar()->hide();
            }
            else
            {
                options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL1, true);
                toolBar()->show();
            }
            break;
    
        case ID_OPTIONS_TOGGLE_TOOL2:
            if(options->isItemChecked(ID_OPTIONS_TOGGLE_TOOL2))
            {
                options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL2, false);
                toolBar("drawToolBar")->hide();
            }
            else
            {
                options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL2, true);
                toolBar("drawToolBar")->show();
            }
            break;
            
        case ID_OPTIONS_TOGGLE_STATS:
            if(options->isItemChecked(ID_OPTIONS_TOGGLE_STATS))
            {
                options->setItemChecked(ID_OPTIONS_TOGGLE_STATS, false);
                statusBar()->hide();
            }
            else
            {
                options->setItemChecked(ID_OPTIONS_TOGGLE_STATS, true);
                statusBar()->show();
            }
            break;
            
        case ID_OPTIONS_SAVE:
            writeConfig();
            break;
        
        default:
            break;
    }
}


void KIconEdit::slotTools( int id )
{
    switch(id)
    {
        case ID_SELECT_RECT:
            grid->setTool(KIconEditGrid::SelectRect);
            break;
            
        case ID_SELECT_CIRCLE:
            grid->setTool(KIconEditGrid::SelectCircle);
            break;
        
        case ID_DRAW_FREEHAND:
            toggleTool(id);
            grid->setTool(KIconEditGrid::Freehand);
            break;
    
        case ID_DRAW_ELLIPSE:
            toggleTool(id);
            grid->setTool(KIconEditGrid::Ellipse);
            break;
    
        case ID_DRAW_ELLIPSE_FILL:
            toggleTool(id);
            grid->setTool(KIconEditGrid::FilledEllipse);
            break;
    
        case ID_DRAW_CIRCLE:
            toggleTool(id);
            grid->setTool(KIconEditGrid::Circle);
            break;
        
        case ID_DRAW_CIRCLE_FILL:
            toggleTool(id);
            grid->setTool(KIconEditGrid::FilledCircle);
            break;
        
        case ID_DRAW_RECT:
            toggleTool(id);
            grid->setTool(KIconEditGrid::Rect);
            break;
        
        case ID_DRAW_RECT_FILL:
            toggleTool(id);
            grid->setTool(KIconEditGrid::FilledRect);
            break;
        
        case ID_DRAW_ERASE:
            toggleTool(id);
            grid->setTool(KIconEditGrid::Eraser);
            break;
        
        case ID_DRAW_LINE:
            toggleTool(id);
            grid->setTool(KIconEditGrid::Line);
            break;
    
        case ID_DRAW_FIND:
            toggleTool(id);
            grid->setTool(KIconEditGrid::Find);
            break;
    
        case ID_DRAW_FILL:
            toggleTool(id);
            grid->setTool(KIconEditGrid::FloodFill);
            break;
    
        case ID_DRAW_SPRAY:
            toggleTool(id);
            grid->setTool(KIconEditGrid::Spray);
            break;

        default:
            if(id >= 500 && id <= 600)
                KMessageBox::sorry(this, i18n("Sorry - not implemented."));
            break;
    }
}


void KIconEdit::slotImage( int id )
{
    switch(id)
    {
        case ID_IMAGE_RESIZE:
            grid->editResize();
            break;

        case ID_IMAGE_GRAYSCALE:
            grid->grayScale();
            break;

        case ID_IMAGE_MAPTOKDE:
            grid->mapToKDEPalette();
            break;

        default:
            break;
    }
}


void KIconEdit::slotSaved()
{
    grid->setModified(false);
}


void KIconEdit::slotUpdateStatusPos(int x, int y)
{
    QString str = i18n("Status Position", "%1, %2").arg(x).arg(y);
    statusbar->changeItem( str, 0);
}


void KIconEdit::slotUpdateStatusSize(int x, int y)
{
    QString str = i18n("Status Size", "%1 x %2").arg(x).arg(y);
    statusbar->changeItem( str, 1);
}

void KIconEdit::slotUpdateStatusScaling(int s, bool show)
{
    props(this)->gridscaling = s;
    QString str;
    
    if(show)
    {
        toolbar->setItemEnabled(ID_VIEW_ZOOM_IN, true);
        toolbar->setItemEnabled(ID_VIEW_ZOOM_OUT, true);
    }
    else
    {
        toolbar->setItemEnabled(ID_VIEW_ZOOM_IN, false);
        toolbar->setItemEnabled(ID_VIEW_ZOOM_OUT, false);
    }

    str.sprintf("1:%d", s);
    statusbar->changeItem( str, 2);
}


void KIconEdit::slotUpdateStatusColors(uint)
{
    QString str = i18n("Colors: %1").arg(grid->numColors());
    statusbar->changeItem( str, 3);
}


void KIconEdit::slotUpdateStatusColors(uint n, uint *)
{
    QString str = i18n("Colors: %1").arg(n);
    statusbar->changeItem( str, 3);
}


void KIconEdit::slotUpdateStatusMessage(const QString & msg)
{
    msgtimer->start(10000, true);
    statusbar->changeItem( msg, 4);
}


void KIconEdit::slotClearStatusMessage()
{
    statusbar->changeItem( "", 4);
}


void KIconEdit::slotUpdateStatusName(const QString & name)
{
    QString cap = "";
    if(!name.isEmpty())
    {
        cap += name;
    }

    // add to list of recent files - this now works -jwc-
    kdDebug() << "KIconEdit::slotUpdateStatusName() name is: " << name << endl;
    addRecent(name);

    // -jwc- do not include app name or dash - kapp takes care of that
    setCaption(cap);
}


void KIconEdit::slotUpdatePaste(bool state)
{
    edit->setItemEnabled(ID_EDIT_PASTE, state);
    toolbar->setItemEnabled(ID_EDIT_PASTE, state);
    edit->setItemEnabled(ID_EDIT_PASTE_AS_NEW, state);
    toolbar->setItemEnabled(ID_EDIT_PASTE_AS_NEW, state);
}


void KIconEdit::slotUpdateCopy(bool state)
{
    edit->setItemEnabled(ID_EDIT_COPY, state);
    toolbar->setItemEnabled(ID_EDIT_COPY, state);
    edit->setItemEnabled(ID_EDIT_CUT, state);
    toolbar->setItemEnabled(ID_EDIT_CUT, state);
}


void KIconEdit::slotOpenBlank(const QSize s)
{
    grid->loadBlank( s.width(), s.height());
}


void KIconEdit::dragEnterEvent(QDragEnterEvent* e)
{
  e->accept(QUriDrag::canDecode(e));
}


/*
    accept drop of a file - opens file in current window
    old code to drop image, as image, should be removed
*/
void KIconEdit::dropEvent( QDropEvent *e )
{
    kdDebug() << "Got QDropEvent!" << endl;

    QStrList fileList;
    bool validUrls = false;
    bool loadedinthis = false;
    
    if(QUriDrag::decode(e, fileList))
    {
        QStrListIterator it(fileList);
        for(; it.current(); ++it)
        {
            kdDebug() << "In dropEvent for "  <<  it.current() << endl;
            KURL url(it.current());
            if(!url.isMalformed())
            {
                validUrls = true;
 
                if (!grid->isModified() && !loadedinthis) 
                {
                    icon->open(&grid->image(), it.current());
                    loadedinthis = true;
                }
                else 
                {
                    slotNewWin(it.current());
                }
            }
        }
    }
}
    

