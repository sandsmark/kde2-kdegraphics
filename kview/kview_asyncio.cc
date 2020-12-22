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

#include "kview_asyncio.h"
#include "canvas.h"

#include <assert.h>

#include <qmovie.h>
#include <qasyncimageio.h>

#include <kio/job.h>
#include <ktempfile.h>

KIOImageSource::KIOImageSource( QIODevice *dev )
{
  m_pBuffer = new char[ 8192 ];
  m_pDev = dev;
  m_iPos = 0;
  m_bEOF = false;
  m_bRewind = false;
}

KIOImageSource::~KIOImageSource()
{
  delete [] m_pBuffer;
  delete m_pDev;
}

int KIOImageSource::readyToSend()
{
  if ( m_pDev->size() == m_iPos )
    return -1;

  if ( m_pDev->status() != IO_Ok )
    return -1;

  return QMIN( 8192, m_pDev->size() - m_iPos );
}

void KIOImageSource::sendTo( QDataSink *sink, int count )
{
  m_pDev->at( m_iPos );
  m_pDev->readBlock( m_pBuffer, count );
  sink->receive( (uchar *)m_pBuffer, count );
  m_iPos += count;
}

bool KIOImageSource::rewindable() const
{
  return true;
}

void KIOImageSource::enableRewind( bool rewind )
{
  m_bRewind = rewind;
}

void KIOImageSource::rewind()
{
  assert( m_bRewind );
  m_iPos = 0;
  m_pDev->reset();
  ready();
}

KIOImageLoader::KIOImageLoader( KIO::Job *job, KVImageHolder *label, KTempFile *temp )
  : m_pLabel(label)
  , m_TempFile(temp)
{
  job->insertChild( this ); //let's die when the job dies ;)

  connect( job, SIGNAL( data( KIO::Job *, const QByteArray &) ),
           this, SLOT( slotData( KIO::Job *, const QByteArray &) ) );
  connect( job, SIGNAL( result( KIO::Job * ) ), this, SLOT( slotResult( KIO::Job * ) ) );

  m_bMovie = false;
  m_bLoading = false;
  m_iBuffPos = 0;
}

void KIOImageLoader::slotData( KIO::Job *, const QByteArray &data )
{
  if ( !m_bLoading )
  {
    /*const char *format =*/ QImageDecoder::formatName( (const uchar *)data.data(), data.size() );

    m_pBuffer = new QBuffer();
    m_pBuffer->open( IO_ReadWrite );

    m_pBuffer->at( m_iBuffPos );
    m_pBuffer->writeBlock( data.data(), data.size() );
    m_iBuffPos += data.size();

    //   if ( format )
    //   {
    //      QMovie movie( new KIOImageSource( m_pBuffer ) );
    //      m_pLabel->setImageMovie( movie );
    //      m_bMovie = true;
    //    }

    m_bLoading = true;
    return;
  }

  //  if ( m_bMovie )
  //  m_pLabel->movie()->pause();

  m_pBuffer->at( m_iBuffPos );
  m_pBuffer->writeBlock( data.data(), data.size() );
  m_iBuffPos += data.size();

  //if ( m_bMovie )
  //  m_pLabel->movie()->unpause();
}

void KIOImageLoader::slotResult( KIO::Job * job )
{
  if (job->error())
    job->showErrorDialog();
  else
  {
    m_pBuffer->close();
    if ( !m_bMovie )
    {
      QPixmap pix;
      pix.loadFromData( m_pBuffer->buffer() );
      m_pLabel->setImagePix( pix );
      if (m_TempFile)
	 {
        m_TempFile->dataStream()->writeRawBytes(m_pBuffer->buffer().data(), m_iBuffPos);
        m_TempFile->close();
	 }
    }
  }
}

#include "kview_asyncio.moc"

