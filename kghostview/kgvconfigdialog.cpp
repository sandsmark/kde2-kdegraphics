/*
 *   kghostview: KDE PostScript viewer
 *   This file: Copyright (C) 2000 Espen Sand, espen@kde.org
 *              Copyright (C) 2001 Wilco Greven, greven@kde.org
 *   Uses ideas from ealier dialog code by Mark Donohoe and Tim Theisen
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

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kinstance.h>
#include <klineedit.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kurlrequester.h>

#include "kgv_view.h"

#include "kgvconfigdialog.h"

KGVConfigDialog::KGVConfigDialog( QWidget *parent, const char *name, 
				  bool modal )
  : KDialogBase( TreeList, i18n( "Configure" ), Ok|Cancel|Default, 
		 Ok, parent, name, modal )
{
    /*== General ==========================================================*/
    QFrame* generalPage = addPage( i18n( "General" ), 
			    i18n( "General settings" ) );
    
    mAntiAliasCheck = new QCheckBox( i18n("Antialiasing"), generalPage );
    mPlatformFontCheck = new QCheckBox( i18n( "Platform fonts" ), generalPage );
    mMessagesCheck = new QCheckBox( i18n( "Messages" ), generalPage );
    mPaletteGroup = new QButtonGroup( 1, Vertical, i18n( "Palette" ), 
				      generalPage );
    mMonoRadio = new QRadioButton( i18n("Monochrome"), mPaletteGroup );
    mGrayscaleRadio = new QRadioButton( i18n("Grayscale"), mPaletteGroup );
    mColorRadio = new QRadioButton( i18n("Color"), mPaletteGroup );
    mBackingGroup = new QButtonGroup( 1, Vertical, i18n( "Backing" ), 
				      generalPage );
    mPixmapRadio = new QRadioButton( i18n("Pixmap"), mBackingGroup );
    mBackingStoreRadio = new QRadioButton( i18n("Backing store"), 
					   mBackingGroup );

    QVBoxLayout* generalLayout = new QVBoxLayout( generalPage, 0, 6 );
    generalLayout->addWidget( mAntiAliasCheck );
    generalLayout->addWidget( mPlatformFontCheck );
    generalLayout->addWidget( mMessagesCheck );
    generalLayout->addWidget( mPaletteGroup );
    generalLayout->addWidget( mBackingGroup );
    generalLayout->addStretch();
    
    connect( mAntiAliasCheck, SIGNAL( toggled( bool ) ),
	     this, SLOT( slotAaToggled( bool ) ) );
    connect( mMonoRadio, SIGNAL( toggled( bool ) ),
	     this, SLOT( slotMonoOrGrayToggled( bool ) ) );
    connect( mGrayscaleRadio, SIGNAL( toggled( bool ) ),
	     this, SLOT( slotMonoOrGrayToggled( bool ) ) );

    /*== Ghostscript ======================================================*/
    QFrame* ghostscriptPage = addPage( i18n( "Ghostscript" ), 
				    i18n( "Ghostscript configuration" ) );
    
    mConfigureButton = new QPushButton( i18n( "Configure..." ), 
					ghostscriptPage );
    QGroupBox* manualConfigBox = new QGroupBox( 1, Horizontal, 
			i18n( "Settings" ), ghostscriptPage );
    // TODO -- buddies for the labels
    new QLabel( i18n( "Interpreter: " ), manualConfigBox );
    mInterpreterReq = new KURLRequester( manualConfigBox);
    new QLabel( i18n( "Non-antialiasing arguments: " ), manualConfigBox );
    mNonAntiAliasArgsEdit = new KLineEdit( manualConfigBox );
    new QLabel( i18n( "Antialiasing arguments: " ), manualConfigBox );
    mAntiAliasArgsEdit = new KLineEdit( manualConfigBox );
    
    QVBoxLayout* layout1 = new QVBoxLayout( ghostscriptPage, 0, 6 );
    QHBoxLayout* layout2 = new QHBoxLayout( layout1 );
    layout1->addWidget( manualConfigBox );
    layout1->addStretch();
    layout2->addWidget( mConfigureButton );
    layout2->addStretch();

    connect( mConfigureButton, SIGNAL( clicked() ),
	     this, SLOT( slotConfigureGhostscript() ) );
    
    readSettings();
}

bool KGVConfigDialog::slotConfigureGhostscript()
{
    kdDebug(4500) << "KGVConfigDialog::slotConfigureGhostscript" << endl; 

    QString exe = "gs";
    if( !KStandardDirs::findExe( exe ) ) {
	return false;
    }
  
    QString versionString; 
    QString chkVersion = exe + " --version";
    FILE* p = popen( chkVersion.local8Bit(), "r" );
    if( p ) {    
	QFile qp;
	qp.open( IO_ReadOnly, p );
	qp.readLine( versionString, 80 );
	qp.close(); 
	pclose( p );
    }
    else
	return false;

    versionString = versionString.stripWhiteSpace();
    
    bool isNumber;
    float versionNum = versionString.toFloat( &isNumber );
    if( !isNumber ) {
	// This shouldn't happen, so no need for a messagebox.
	return false;
    }

    mInterpreterPath = exe;
    mNonAntiAliasArgs = "-sDEVICE=x11";
    mAntiAliasArgs =  versionNum < 6.50 
		? "-sDEVICE=x11alpha"
		: "-sDEVICE=x11 -dTextAlphaBits=4 -dGraphicsAlphaBits=2 " 
		  "-dMaxBitmap=10000000";
   
    setup();
    
    return true;
}

void KGVConfigDialog::slotAaToggled( bool on )
{
    if( on )
	mColorRadio->setChecked( true );
}

void KGVConfigDialog::slotMonoOrGrayToggled( bool on )
{
    if( on ) 
	mAntiAliasCheck->setChecked( false );
}

void KGVConfigDialog::setup()
{
    mAntiAliasCheck->setChecked( mAntialias ); 
    mPlatformFontCheck->setChecked( mPlatformFonts ); 
    mMessagesCheck->setChecked( mShowMessages ); 

    mMonoRadio->setChecked( mPaletteType == MONO_PALETTE );
    mGrayscaleRadio->setChecked( mPaletteType == GRAY_PALETTE ); 
    mColorRadio->setChecked( mPaletteType == COLOR_PALETTE );

    mPixmapRadio->setChecked( mBackingType == PIX_BACKING );
    mBackingStoreRadio->setChecked( mBackingType == STORE_BACKING );

    mInterpreterReq->setURL( mInterpreterPath );
    mNonAntiAliasArgsEdit->setText( mNonAntiAliasArgs );
    mAntiAliasArgsEdit->setText( mAntiAliasArgs );
}

void KGVConfigDialog::slotOk()
{
    // mInterpreterPath  = mInterpreterEdit->text();
    mAntialias = mAntiAliasCheck->isChecked();
    mShowMessages = mMessagesCheck->isChecked();
    mPlatformFonts = mPlatformFontCheck->isChecked();
    if( mColorRadio->isChecked() )
	mPaletteType = COLOR_PALETTE;
    else if( mGrayscaleRadio->isChecked() )
	mPaletteType = GRAY_PALETTE;
    else
	mPaletteType = MONO_PALETTE;

    if( mPixmapRadio->isChecked() )
	mBackingType = PIX_BACKING;
    else
	mBackingType = STORE_BACKING;

    mInterpreterPath = mInterpreterReq->url();
    mNonAntiAliasArgs = mNonAntiAliasArgsEdit->text();
    mAntiAliasArgs = mAntiAliasArgsEdit->text();

    writeSettings();
    accept();
}


void KGVConfigDialog::slotDefault()
{
    mInterpreterPath	= "gs";
    mAntialias		= true;
    mShowMessages	= true;
    mPlatformFonts	= false;
    mPaletteType	= COLOR_PALETTE;
    mBackingType	= PIX_BACKING;
    setup();
}

void KGVConfigDialog::readSettings()
{
    kdDebug(4500) << "KGVConfigDialog::readSettings" << endl; 
    
    KConfig* config = KGVFactory::instance()->config();
    QString oldGroup = config->group();
    
    config->setGroup( "General" );
  
    mAntialias = config->readBoolEntry( "Antialiasing", true );
    mShowMessages = config->readBoolEntry( "Messages", false );
    mPlatformFonts = config->readBoolEntry( "Platform fonts", false );

    QString text = config->readEntry( "Palette" );
    if( text.find( "mono" ) == 0 )
	mPaletteType = MONO_PALETTE;
    else if( text.find( "gray" ) == 0 )
	mPaletteType = GRAY_PALETTE;
    else
	mPaletteType = COLOR_PALETTE;
  
    text = config->readEntry( "Backing" );
    if( text.find( "store" ) == 0 )
	mBackingType = STORE_BACKING;
    else
	mBackingType = PIX_BACKING;
    
    if( config->hasGroup( "Ghostscript" ) ) {
	config->setGroup( "Ghostscript" );
	mInterpreterPath = config->readPathEntry( "Interpreter" );
	mNonAntiAliasArgs = config->readEntry( "Non-antialiasing arguments" );
	mAntiAliasArgs = config->readEntry( "Antialiasing arguments" );
    }
    else if( slotConfigureGhostscript() ) {
	config->setGroup( "Ghostscript" );
	config->writeEntry( "Interpreter", mInterpreterPath );
	config->writeEntry( "Non-antialiasing arguments", mNonAntiAliasArgs );
	config->writeEntry( "Antialiasing arguments", mAntiAliasArgs );
	config->sync(); 
    }
    
    config->setGroup( oldGroup );
	
    setup();
}

void KGVConfigDialog::writeSettings()
{
    kdDebug(4500) << "KGVConfigDialog::writeSettings" << endl; 
    
    KConfig* config = KGVFactory::instance()->config();
    QString oldGroup = config->group();
    
    config->setGroup( "General" );

    config->writeEntry( "Interpreter", mInterpreterPath );
    config->writeEntry( "Antialiasing", mAntialias );
    config->writeEntry( "Platform fonts", mPlatformFonts );
    config->writeEntry( "Messages", mShowMessages );

    QString text;
  
    if( mPaletteType == COLOR_PALETTE )
	text = "color";
    else if( mPaletteType == GRAY_PALETTE )
	text = "grayscale";
    else
	text = "monochrome";
    config->writeEntry( "Palette", text );
  
    if( mBackingType == PIX_BACKING )
	text = "pixmap";
    else
	text = "store";
    config->writeEntry( "Backing", text );

    config->setGroup( "Ghostscript" );
    config->writeEntry( "Interpreter", mInterpreterPath );
    config->writeEntry( "Non-antialiasing arguments", mNonAntiAliasArgs );
    config->writeEntry( "Antialiasing arguments", mAntiAliasArgs );

    config->setGroup( oldGroup );
    config->sync();
}

#include "kgvconfigdialog.moc"
