/***************************************************************************
                          scansourcedialog.cpp  -  description
                             -------------------
    begin                : Sun Jan 16 2000
    copyright            : (C) 2000 by Klaas Freitag
    email                : freitag@suse.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "scansourcedialog.h"
#include "kscanslider.h"

#include <klocale.h>
#include <kdebug.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qhbox.h>

#include <qvbuttongroup.h>


extern "C"{
#include <sane/saneopts.h>
}
#ifndef SANE_NAME_DOCUMENT_FEEDER
#define SANE_NAME_DOCUMENT_FEEDER "Automatic Document Feeder"
#endif


ScanSourceDialog::ScanSourceDialog( QWidget *parent, const QStrList list, ADF_BEHAVE adfBehave )
 : KDialogBase( parent, "SOURCE_DIALOG", true, i18n("Scan Source Selection"),
		Ok|Cancel,Ok, true)
{
   QVBox *vbox = makeVBoxMainWidget();

   (void) new QLabel( i18n("<B>Source selection</B><P>"
			   "Mind that you might see more sources than really exist"), vbox );

   /* Combo Box for sources */
   const QStrList xx = list;
   sources = new KScanCombo( vbox,
			     i18n("Select the Scanner document source:"),
			     xx);
   connect( sources, SIGNAL( activated(int)), this, SLOT( slChangeSource(int)));


   /* Button Group for ADF-Behaviour */
   bgroup = 0;
   adf = ADF_OFF;

   if( sourceAdfEntry() > -1 )
   {
      bgroup = new QVButtonGroup( i18n("Advanced ADF-Options:"), vbox, "ADF_BGROUP" );

      connect( bgroup, SIGNAL(clicked(int)), this, SLOT( slNotifyADF(int)));

      /* Two buttons inside */
      QRadioButton *rbADFTillEnd = new QRadioButton( i18n("Scan until ADF reports out of paper"),
						  bgroup );
      bgroup->insert( rbADFTillEnd, ADF_SCAN_ALONG );

      QRadioButton *rbADFOnce = new QRadioButton( i18n("Scan only one sheet of ADF per click"),
					       bgroup );
      bgroup->insert( rbADFOnce, ADF_SCAN_ONCE );

      switch ( adfBehave )
      {
	 case ADF_OFF:
	    bgroup->setButton( ADF_SCAN_ONCE );
	    bgroup->setEnabled( false );
	    adf = ADF_OFF;
	    break;
	 case ADF_SCAN_ONCE:
	    bgroup->setButton( ADF_SCAN_ONCE );
	    adf = ADF_SCAN_ONCE;
	    break;
	 case ADF_SCAN_ALONG:
	    bgroup->setButton( ADF_SCAN_ALONG );
	    adf = ADF_SCAN_ALONG;
	    break;
	 default:
	    kdDebug(29000) << "Undefined Source !" << endl;
	    // Hmmm.
	    break;
      }
   }
}

QString  ScanSourceDialog::getText( void ) const
{
   return( sources->currentText() );
}

void ScanSourceDialog::slNotifyADF( int adf_group )
{
  // debug( "reported adf-select %d", adf_group );
#ifdef __GNUC__
#warning this seems to be broken, adf_text is a visible string?
#endif
#if 0    
  QString adf_text = getText();
	
  adf = ADF_OFF;
	
  if( adf_text == "Automatic Document Feeder" ||
      adf_text == "ADF" )
    {
      if( adf_group == 0 )
	adf = ADF_SCAN_ALONG;
      else
	adf = ADF_SCAN_ONCE;
    }
#endif
}


void ScanSourceDialog::slChangeSource( int i )
{
   if( ! bgroup ) return;

   if( i == sourceAdfEntry())
   {
      /* Adf was switched on */
      bgroup->setEnabled(  true );
      bgroup->setButton( 0 );
      adf = ADF_SCAN_ALONG;
      adf_enabled = true;
   }
   else
   {
      bgroup->setEnabled( false );
      // adf = ADF_OFF;
      adf_enabled = false;
   }
}



int ScanSourceDialog::sourceAdfEntry( void ) const
{
   if( ! sources ) return( -1 );

   int cou = sources->count();

   for( int i = 0; i < cou; i++ )
   {
      QString q = sources->text( i );

#ifdef __GNUC__
#warning same here
#endif

#if 0
      if( q == "ADF" || q == SANE_NAME_DOCUMENT_FEEDER )
         return( i );
#endif
      
   }
   return( -1 );
}



void ScanSourceDialog::slSetSource( const QString source )
{
   if( !sources  ) return;
   kdDebug(29000) << "Setting <" << source << "> as source" << endl;

   if( bgroup )
      bgroup->setEnabled( false );
   adf_enabled = false ;


   for( int i = 0; i < sources->count(); i++ )
   {
      if( sources->text( i ) == source )
      {
         sources->setCurrentItem( i );
         if( source == QString::number(sourceAdfEntry()) )
         {
	    if( bgroup )
	       bgroup->setEnabled( true );
            adf_enabled = true;
         }
         break;
      }
   }

}


ScanSourceDialog::~ScanSourceDialog()
{

}

/* EOF */
#include "scansourcedialog.moc"
