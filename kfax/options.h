/*
    $Id: options.h 30450 1999-10-09 17:17:05Z bieker $

       
    Copyright (C) 1996 Bernd Johannes Wuebben   
                       wuebben@math.cornell.edu

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


#ifndef _OPTIONS_DIALOG_H_
#define _OPTIONS_DIALOG_H_

#include <qapplication.h>
#include <qfiledialog.h>
#include <qframe.h> 
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qscrollbar.h>
#include <qtooltip.h>

#include <qstring.h>


struct optionsinfo {
  int geomauto;
  int resauto;
  int width ;
  int height;
  int fine;
  int landscape;
  int flip;
  int invert;
  int lsbfirst;
  int raw;
};

class KIntNumInput;

class OptionsDialog : public QDialog {

    Q_OBJECT

public:
    OptionsDialog( QWidget *parent = 0, const char *name = 0);

    struct optionsinfo* getInfo();
    void setWidgets(struct optionsinfo *oi);

protected:
    void focusInEvent ( QFocusEvent *);

signals:

public slots:

      void ready();
      void help();
      void geomtoggled();
      void g32toggled();
      void g4toggled();
      void g3toggled();
      
private:

    QGroupBox	 *bg;
    QLabel 	*reslabel;
    QButtonGroup *resgroup;
    QRadioButton *fine;
    QRadioButton *resauto;
    QRadioButton *normal;
    QLabel	*displaylabel;
    QButtonGroup *displaygroup;
    QCheckBox *landscape;
    QCheckBox *geomauto;
    QCheckBox *flip;
    QCheckBox *invert;

    QButtonGroup  *lsbgroup;
    QLabel        *lsblabel;
    QCheckBox *lsb;
    QButtonGroup *rawgroup;	
    QRadioButton *g3;
    QRadioButton *g32d;
    QRadioButton *g4;

    QLabel 	*rawlabel;
    
    QLabel 	*widthlabel;
    QLabel 	*heightlabel;
    KIntNumInput *widthedit;
    KIntNumInput *heightedit;
    
    QPushButton	 *ok_button;
    QPushButton	 *cancel_button;
    QPushButton	 *helpbutton;
    
    struct optionsinfo oi;

};


#endif
