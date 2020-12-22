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

#include <stdlib.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kio/netaccess.h>
#include <kimageio.h>
#include <kdebug.h>

#include "kicon.h"


KIconEditIcon::KIconEditIcon(QObject *parent, const QImage *img, const QString &filename) 
   : QObject(parent)
{
    f = 0;
    _lastdir = "/";
    //checkUnNamedBackup(img);

    if(!filename.isEmpty())
    {
        open(img, filename);
    }
}


KIconEditIcon::~KIconEditIcon()
{
}


void KIconEditIcon::open(const QImage *image, const QString &xpmName)
{
    QImage *img = (QImage*)image;
    QString filename(xpmName);
    KURL _turl(filename);

    if(filename.isEmpty())
        return;

    kdDebug() << "KIconEditIcon::open " << filename << endl;
    
    QString tmp = xpmName;

    if(_turl.isMalformed()) // try to see if it is a relative filename
    {
        kdDebug() << "KIconEditIcon::open (malformed) " << filename << endl;

        QFileInfo fi(filename);
        if(fi.isRelative())
            filename = "file:" + fi.absFilePath();
        _turl = filename;

        if(_turl.isMalformed()) // Giving up
        {
            QString msg = i18n("The URL: %1 \nseems to be malformed.\n").arg(_turl.url());
            KMessageBox::sorry((QWidget*)parent(), msg);
            return;
        }
    }

    if(!img->load(_turl.path())) 
    {
        QString msg = i18n("KIcon: There was a QT error loading:\n%1\n").arg(_turl.path());
        KMessageBox::error((QWidget*)parent(), msg);
        return;
    }

    kdDebug() << "KIconEditIcon::open - Image loaded" << endl;
    
    // _url is saved off for use in saving the image to the same 
    // file later - should include full path
    _url = _turl.url();
    kdDebug() << "KIcon: _url: " << _url << endl;

    // this causes updates of preview, color palettes, etc.    
    emit loaded(img);
    kdDebug() << "loaded(img)" <<  endl;    
    
    // this is the name that shows up in status bar - 
    // should be filename with path
    emit newname(_turl.path()); 
    kdDebug() << "newname(_url) : " << _url << endl;    
   
    kdDebug() << "KIconEditIcon::open - done" << endl;
}



void KIconEditIcon::promptForFile(const QImage *img)
{
    kdDebug() << "KIconEditIcon::promptForFile(const QImage *img)" << endl;
    /*
    QString filter = i18n("*|All files (*)\n"
                        "*.xpm|XPM (*.xpm)\n"
                        "*.png|PNG (*.png)\n"
                        "*.gif|GIF files (*.gif)\n"
                        "*.jpg|JPEG files (*.jpg)\n"
                        "*.ico|Icon files (*.ico)\n");

  
    KURL url = KFileDialog::getOpenURL( QString::null, filter );
    */
    KURL url = KFileDialog::getImageOpenURL( QString::null );

    if( url.isEmpty() )
        return;
  
    QString tempFile;

    KIO::NetAccess::download( url, tempFile );
    open( img, tempFile );
    KIO::NetAccess::removeTempFile( tempFile );
}



void KIconEditIcon::saveAs(const QImage *image)
{
    kdDebug() << "KIconEditIcon::saveAs" << endl;

    /* note - free Qt will open, but not save, GIF format
    no great loss */

    QString filter = i18n("*|All files (*)\n"
                        "*.xpm|XPM (*.xpm)\n"
                        "*.png|PNG (*.png)\n"
                        "*.jpg|JPEG files (*.jpg)\n"
                        "*.ico|Icon files (*.ico)\n");

    KURL url = KFileDialog::getSaveURL(QString::null, filter);

    if( url.isEmpty() )
        return;
  
    if( !url.isLocalFile() )
    {
        KMessageBox::sorry( 0L, i18n( "Only saving locally is supported." ) );
        return;
    }
    
    save( image, url.path() );
}



void KIconEditIcon::save(const QImage *image, const QString &_filename)
{
    kdDebug() << "KIconEditIcon::save" << endl;
    QString filename = _filename;

    if(filename.isEmpty())
    {
        if(_url.isEmpty())
        {
            saveAs(image);
            return;
        }
        else
        {
            KURL turl(_url);
            filename = turl.path();
        }    
    }

    QImage *img = (QImage*)image;
    img->setAlphaBuffer(true);
    
    /* jwc - need to fix transparence in reverse by 
     converting normal background color to 00 in alpha 
     channel - see fixTransparence in kicongrid.cpp 
     
     TRANSPARENT is defined as :
        QApplication::palette().normal().background().rgb()
     
     OPAQUE_MASK is defined as :
        0xff000000
     */
    
    for(int y = 0; y < img->height(); y++)
    {
        uint *l = (uint*)img->scanLine(y);
        for(int x = 0; x < img->width(); x++, l++)
        {
            if(*l == TRANSPARENT)
            {
                *l &= ~(OPAQUE_MASK);
            }
        }
    }

    KURL turl(filename);
    QString str = turl.path();
    
    /* base image type on file extension - let kimageio
    take care of this determination */
    
    if(img->save(str, KImageIO::type(str).ascii()))    
    {
        kdDebug() << "img->save()) successful" << endl;
        emit saved();    
        _url = filename;
        
        // emit signal to change title bar to reflect new name
        emit newname(filename); 
        kdDebug() << "newname(filenamme) : " << _url << endl;    
        
    }
    else 
    {
        QString msg = i18n("There was an error saving:\n%1\n").arg(_url);
        KMessageBox::error((QWidget*)parent(), msg);
        kdDebug() << "KIconEditIcon::save - " << msg << endl;
    }

    kdDebug() << "KIconEditIcon::save - done" << endl;    
}


#include "kicon.moc"
