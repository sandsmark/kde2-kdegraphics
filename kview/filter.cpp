/*
* filter.cpp -- Implementation of class KImageFilter.
* Author:       Sirtaj Singh Kang
* Version:      $Id: filter.cpp 74281 2000-12-10 16:56:09Z mkretz $
* Generated:    Tue Apr  7 18:21:41 EST 1998
*/

#include"filter.h"

void KImageFilter::setMaxProgress( int max )
{
  _max = max;
}

void KImageFilter::setProgress( int val )
{
  if( _max == 0 )
    return;

  long pct = ((long)val * 100) / (long)_max;

  emit progress( (int)pct );
}

void KImageFilter::slotRaiseFilter()
{
  emit selected( this );
}

#include "filter.moc"

