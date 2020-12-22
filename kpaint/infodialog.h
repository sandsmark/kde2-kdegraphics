// -*- c++ -*-

// $Id: infodialog.h 107633 2001-07-25 15:35:16Z rich $

#ifndef IMAGEINFODIALOG_H
#define IMAGEINFODIALOG_H

#include <qcombobox.h>
#include <qlabel.h>
#include <kdialog.h>

class Canvas;

class imageInfoDialog : public KDialog
{
    Q_OBJECT

public:

    imageInfoDialog(Canvas *c, QWidget* parent= 0, const char* name= 0);

private:
    QLabel* colourDepth;
    QLabel* coloursUsed;
    QLabel* width;
    QLabel* height;
};

#endif // IMAGEINFODIALOG_H
