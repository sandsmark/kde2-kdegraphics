/*
    KDE Icon Editor - a small icon drawing program for the KDE.
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

#ifndef __KICON_H__
#define __KICON_H__

#include <qdir.h>
#include <qobject.h>
#include <qimage.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <kapp.h>
#include <kfiledialog.h>
#include "kcolorgrid.h"
#include "utils.h"

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(HAVE_FCNTL_H) && !defined(HAVE_FLOCK)
#include <fcntl.h>
#endif

#ifdef HAVE_STDC_HEADERS
#include <stdlib.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>


class KIconEditIcon : public QObject
{
  Q_OBJECT
public:
  KIconEditIcon(QObject*, const QImage*, const QString &filename = QString::null );
  ~KIconEditIcon();

  bool isLocal() { return local; }
  QString url() { return _url; }

public slots:
  void open(const QImage*, const QString &xpm= QString::null);
  void promptForFile(const QImage*);
  void save(const QImage*, const QString &filename=QString::null);
  void saveAs(const QImage*);

signals:
  void newmessage( const QString &);
  void newname(const QString &);
  void opennewwin(const QString &);
  void loaded(QImage *);
  void saved();

protected:
    bool local;
    QString _url;
    QString _lastdir;
    FILE *f;
};

#endif //__KICON_H__
