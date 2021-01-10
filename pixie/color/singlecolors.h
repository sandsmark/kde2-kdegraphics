#ifndef __KIF_SINGLECOLOREDIT_H
#define __KIF_SINGLECOLOREDIT_H

#include "widgets.h"
#include <qlistbox.h>
#include <qdialog.h>
#include <qlist.h>
#include <qimage.h>
#include <qlabel.h>
#include <qsemimodal.h>
#include <qgroupbox.h>
#include <kprogress.h>

class KIFImageWindow;
class KColorButton;


class KIFColorListItem : public QListBoxText
{
public:
    KIFColorListItem(QListBox *listBox, const QString &text, const QColor &c);
    const QColor color(){return(colorBrush.color());}
    void setColor(const QColor &c);
    int width(const QListBox *);
    int height(const QListBox *);
protected:
    virtual void paint(QPainter *p);

    QBrush colorBrush;
};

class KIFRgbList : public QList<unsigned int>
{
public:
    KIFRgbList(){;}
    int compareItems(Item s1, Item s2);
};

// use QProgressDialog when themed
class KIFColorProgress : public QSemiModal
{
public:
    KIFColorProgress(const QString &text, QWidget *parent=0,
                     const char *name=0);
    void setValue(int val){progress->setValue(val);}
    void setLabel(const QString &str){label->setText(str);}
protected:
    KProgress *progress;
    QLabel *label;
};

class KIFSingleColorEdit : public QDialog
{
	Q_OBJECT
public:
    KIFSingleColorEdit(QWidget *parent=0, const char *name=0);
protected slots:
    void slotRGBChanged(int r, int g, int b);
    void slotColorBtnChanged(const QColor &c);
    void slotSelected(QListBoxItem *i);
    void slotRevert();
protected:
    void replaceCurrent(const QColor &newColor);
    
    QImage origImage;
    QListBox *colorList;
    ColorSelector *colorSelect;
    KColorButton *colorBtn;
};

#endif

