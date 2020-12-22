/*
 *   kghostview: KDE PostScript viewer
 *   This file: Copyright (C) 2000 Espen Sand, espen@kde.org
 *   Uses ideas from ealier dialog code 
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

// Add header files alphabetically

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qvalidator.h>

#include <kapp.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "gotodialog.h"

GotoDialogData::GotoDialogData( int currentPage, int numSections,
			        int *pagesInSection )
{
  mCurrentPage = currentPage;
  mNumSections = QMIN( 10, numSections );
  for( int i=0; i<mNumSections; mPagesInSection[i]=pagesInSection[i], i++ );

  int sumPages = 0;
  for( int i=0; i<mNumSections; i++ )
  {
    if( sumPages + mPagesInSection[i] > mCurrentPage )
    {
      mCurrentSection = i+1;
      return;
    }
    sumPages += mPagesInSection[i];
  } 
  mCurrentSection = 1;
}


GotoDialogData::GotoDialogData()
{
  mCurrentPage = 0;
  mNumSections = 0;
  mCurrentSection = 0;
  memset( mPagesInSection, 0, sizeof(mPagesInSection) );
}


void GotoDialogData::setPageAndSection( int page, int section )
{
  section = QMAX( 1, section );
  page    = QMAX( 1, page );

  if( section > mNumSections )
  {
    section = mNumSections;
  }

  if( page > mPagesInSection[section-1] )
  {
    page = mPagesInSection[section-1];
  }

  mCurrentSection = section;
  mCurrentPage    = page;
  for( int i=0; i<(mCurrentSection-1); i++ )
  {
    mCurrentPage += mPagesInSection[i];
  }
}


int GotoDialogData::currentSection() const
{
  return mCurrentSection;
}


int GotoDialogData::numSection() const
{
  return mNumSections;
}


int GotoDialogData::currentPage() const
{
  return mCurrentPage;
}


int GotoDialogData::currentPageInCurrentSection() const
{
  int sumPages = 0;
  for( int i=0; i<mNumSections; i++ )
  {
    if( sumPages + mPagesInSection[i] >= mCurrentPage )
    {
      return mCurrentPage - sumPages;
    }
    sumPages += mPagesInSection[i];
  } 
  return 1;
}


int GotoDialogData::numPage( int section ) const
{
  if( section > 0 ) { section -= 1; }
  return( section > mNumSections ? 1 : mPagesInSection[section] );
}






GotoDialog::GotoDialog( QWidget *parent, const char *name, bool modal)
  :KDialogBase( parent, name, modal, i18n("Go to page"), Ok|Apply|Close, Ok, 
		true )
{
  QFrame *page = makeMainWidget();
  QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );

  QGridLayout *glay = new QGridLayout( topLayout, 2, 3 );
  glay->addColSpacing( 1, fontMetrics().maxWidth()*15 );
  
  QLabel *label = new QLabel( i18n("Section:"), page );
  glay->addWidget( label, 0, 0, AlignRight );
  mSectionEdit = new QLineEdit( page );
  glay->addWidget( mSectionEdit, 0, 1 );
  mSectionMaxLabel = new QLabel( page );
  glay->addWidget( mSectionMaxLabel, 0, 2 );

  label = new QLabel( i18n("Page:"), page );
  glay->addWidget( label, 1, 0, AlignRight );
  mPageEdit = new QLineEdit( page );
  glay->addWidget( mPageEdit, 1, 1 );
  mPageMaxLabel = new QLabel( page );
  glay->addWidget( mPageMaxLabel, 1, 2 );
  
  topLayout->addStretch(1);

  QIntValidator *validator = new QIntValidator(mSectionEdit);
  validator->setBottom(1);
  mSectionEdit->setValidator( validator );
  mPageEdit->setValidator( validator );
}


void GotoDialog::setup( const GotoDialogData &dialogData )
{
  mDialogData = dialogData;

  int curSection = mDialogData.currentSection();
  int numSection = mDialogData.numSection();
  int curPage    = mDialogData.currentPageInCurrentSection();
  int numPage    = mDialogData.numPage( curSection );

  mSectionEdit->setText( QString::number(curSection) );
  mSectionMaxLabel->setText( i18n("of %1").arg(numSection) ); 
  mPageEdit->setText( QString::number(curPage) );
  mPageMaxLabel->setText( i18n("of %1").arg(numPage) );

  if (numSection == 1)
    mSectionEdit->setEnabled (false);
}


void GotoDialog::slotOk()
{
  if( validateSetting() )
  {
    // kghostview expects the first page to start at 0
    int page = mDialogData.currentPage();
    emit gotoPage( page > 0 ? page-1 : page );
    accept();
  }
}


void GotoDialog::slotApply()
{
  if( validateSetting() )
  {
    int page = mDialogData.currentPage();
    emit gotoPage( page > 0 ? page-1 : page );
  }
}


bool GotoDialog::validateSetting()
{
  QString sectionText = mSectionEdit->text().stripWhiteSpace();
  QString pageText = mPageEdit->text().stripWhiteSpace();

  if( sectionText.isEmpty() == true || pageText.isEmpty() == true )
  {
    QString msg = i18n( "You must enter section and page numbers first." );
    KMessageBox::sorry( this, msg );
    return false;
  }
 
  bool sectionValid, pageValid;
  uint section = sectionText.toUInt( &sectionValid );
  uint page    = pageText.toUInt( &pageValid );

  if( sectionValid == false || pageValid == false )
  {
    QString msg = i18n( "Please enter a valid number first." );
    KMessageBox::sorry( this, msg );
    return false;
  }

  mDialogData.setPageAndSection( page, section );
  setup( mDialogData );

  return true;
}
#include "gotodialog.moc"
