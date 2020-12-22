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
#ifndef __kview_view_h__
#define __kview_view_h__

#include <kparts/browserextension.h>

#include <kparts/factory.h>

#include "canvas.h"

namespace KIO { class Job; class SimpleJob; }

class KInstance;
class KAction;
class KViewKonqExtension;
class KTempFile;

class KViewFactory : public KParts::Factory
{
  Q_OBJECT
public:
  KViewFactory();
  virtual ~KViewFactory();

  virtual KParts::Part *createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *classname, const QStringList &args );


  static KInstance *instance();

private:
  static KInstance *s_instance;
};


class KViewPart: public KParts::ReadOnlyPart
{
  Q_OBJECT
public:
  KViewPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name );
  virtual ~KViewPart();

  virtual bool openURL( const KURL &url );
  virtual bool closeURL();

  KImageCanvas *canvas() const { return m_pCanvas; }

protected:
  virtual bool openFile() { return false; } // not called

protected slots:
  void slotResult( KIO::Job * );
//void slotRedirection( int, const QString & );

  void slotZoomIn();
  void slotZoomOut();
  void slotRotate();
  void slotReset();

  virtual void keyPressEvent( QKeyEvent *ev );
// removed resizeEvent...

  void slotPopupMenu( const QPoint &pos );

  void slotSaveImageAs();

  void slotResultSaveImageAs( KIO::Job *job );

private:
  KIO::SimpleJob * m_job;
  KViewKonqExtension * m_extension;
  QWMatrix matrix;
  int zoomFactor;
  KImageCanvas *m_pCanvas;

  KAction *m_paZoomIn;
  KAction *m_paZoomOut;
  KAction *m_paRotate;
  KAction *m_paReset;

  QString m_popupDoc;
  QString m_mimeType;

  KTempFile *m_TempFile;
};

class KViewKonqExtension: public KParts::BrowserExtension
{
  Q_OBJECT
  friend class KViewPart; // it can emit our signals
public:
  KViewKonqExtension( KViewPart *parent, const char *name = 0 );
  virtual ~KViewKonqExtension() {}

  virtual void setXYOffset( int x, int y );
  virtual int xOffset();
  virtual int yOffset();

  KImageCanvas *canvas() { return ((KViewPart *)parent())->canvas(); }

public slots:
  // Automatically detected by konqueror
  void print();

};

#endif
