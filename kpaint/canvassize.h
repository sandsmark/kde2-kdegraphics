// $Id: canvassize.h 107633 2001-07-25 15:35:16Z rich $

#ifndef CANVASSIZE_H
#define CANVASSIZE_H

#include <qlabel.h>
#include <qcheckbox.h>

#include <kdialogbase.h>
#include <klineedit.h>

class canvasSizeDialog : public KDialogBase
{
   Q_OBJECT
public:
   canvasSizeDialog(QWidget *parent=0, const char *name= 0,int _width=0,int _height=0);
   int getWidth();
   int getHeight();
private:
   QLabel *widthLabel;
   QLabel *heightLabel;
   KLineEdit *widthEdit;
   KLineEdit *heightEdit;
   QCheckBox *pictureRatio;
   int width;
   int height;
public slots:
   void slotWidthTextChanged(const QString &);
   void changeState();
};

#endif // CANVASSIZE_H
