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

#ifndef __kview_asyncio_h__
#define __kview_asyncio_h__ "$Id: kview_asyncio.h 111889 2001-08-26 05:53:41Z neil $"

#include <qasyncio.h>
#include <qbuffer.h>

class KVImageHolder;
class KTempFile;
namespace KIO { class Job; }

class KIOImageSource : public QDataSource
{
public:
  KIOImageSource( QIODevice *dev );
  ~KIOImageSource();

  int readyToSend();

  void sendTo( QDataSink *sink, int count );

  bool rewindable() const;

  void enableRewind( bool rewind );

  void rewind();

private:
  char *m_pBuffer;
  QIODevice *m_pDev;
  unsigned int m_iPos;
  bool m_bEOF;
  bool m_bRewind;
};

class KIOImageLoader : QObject
{
  Q_OBJECT
public:
  KIOImageLoader( KIO::Job *job, KVImageHolder *label, KTempFile *temp = 0 );

private slots:
  void slotData( KIO::Job * job , const QByteArray &data );
  void slotResult( KIO::Job * job );

private:
  bool m_bLoading;
  bool m_bMovie;
  int m_iBuffPos;
  QBuffer *m_pBuffer;
  KVImageHolder *m_pLabel;
  KTempFile *m_TempFile;
};

#endif
