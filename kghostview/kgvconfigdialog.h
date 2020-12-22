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
#ifndef _INTERPRETER_DIALOG_H_
#define _INTERPRETER_DIALOG_H_

#include <qstring.h>
#include <kdialogbase.h>

class QButtonGroup;
class QCheckBox;
class QPushButton;
class QRadioButton;
class KLineEdit;
class KURLRequester;

class KGVConfigDialog : public KDialogBase 
{
    Q_OBJECT

public:
    enum {COLOR_PALETTE, GRAY_PALETTE, MONO_PALETTE};
    enum {PIX_BACKING, STORE_BACKING};

    KGVConfigDialog( QWidget* parent = 0, const char* name = 0, 
		     bool modal = true );
    void setup();

    void readSettings();
    void writeSettings();

    bool    antiAlias()		const { return mAntialias; }
    bool    showMessages()	const { return mShowMessages; }
    bool    platformFonts()	const { return mPlatformFonts; }
    int	    paletteType()	const { return mPaletteType; }
    int	    backingStoreType()	const { return mBackingType; }
    
    QString interpreterPath()	const { return mInterpreterPath; }
    QString nonAntialiasArgs()	const { return mNonAntiAliasArgs; }
    QString antialiasArgs()	const { return mAntiAliasArgs; }

protected slots:
    virtual void slotOk();
    virtual void slotDefault();
    
    bool slotConfigureGhostscript();
    void slotAaToggled( bool );
    void slotMonoOrGrayToggled( bool );

private:
    QCheckBox* mAntiAliasCheck;
    QCheckBox* mPlatformFontCheck;
    QCheckBox* mMessagesCheck;
    QRadioButton* mMonoRadio;
    QRadioButton* mGrayscaleRadio;
    QRadioButton* mColorRadio;
    QRadioButton* mPixmapRadio;
    QRadioButton* mBackingStoreRadio;
    QButtonGroup* mPaletteGroup;
    QButtonGroup* mBackingGroup;

    QPushButton* mConfigureButton;
    KURLRequester* mInterpreterReq;
    KLineEdit* mNonAntiAliasArgsEdit;
    KLineEdit* mAntiAliasArgsEdit;

    bool    mAntialias;
    bool    mShowMessages;
    bool    mPlatformFonts;
    int	    mPaletteType;
    int	    mBackingType;
    
    QString mInterpreterPath;
    QString mNonAntiAliasArgs;
    QString mAntiAliasArgs;
};

#endif 

