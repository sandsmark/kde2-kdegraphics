#include "singlecolors.h"
#include "init.h"
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qlayout.h>
#include <qvgroupbox.h>

#include <klocale.h>
#include <kbuttonbox.h>
#include <kcolorbtn.h>
#include <kseparator.h>
#include <kapp.h>
#include <kdebug.h>
    
int KIFRgbList::compareItems(Item item1, Item item2)
{
    unsigned int *c1 = (unsigned int *)item1;
    unsigned int *c2 = (unsigned int *)item2;
    if(*c1 > *c2)
        return(1);
    else if (*c1 < *c2)
        return(-1);
    return(0);
}

KIFColorListItem::KIFColorListItem(QListBox *listBox, const QString &text,
                                   const QColor &c)
    : QListBoxText(listBox, text)
{
    colorBrush = QBrush(c, QBrush::SolidPattern);
}

int KIFColorListItem::height(const QListBox *listBox)
{
    return(listBox ? listBox->fontMetrics().lineSpacing()+2 >= 20 ?
           listBox->fontMetrics().lineSpacing()+2 : 20 : 0);
}

int KIFColorListItem::width(const QListBox *listBox)
{
    return(listBox ? listBox->fontMetrics().width(text())+22 : 0);
}

void KIFColorListItem::paint(QPainter *p)
{
    QFontMetrics fm = p->fontMetrics();
    p->fillRect(2, 2, 16, 16, colorBrush);
    p->drawText(20, fm.ascent()+fm.leading()/2, text());
}

void KIFColorListItem::setColor(const QColor &c)
{
    colorBrush.setColor(c);
    QString cName;
    cName.sprintf("#%.2x%.2x%.2x", c.red(), c.green(), c.blue());
    setText(cName);
}
    
KIFColorProgress::KIFColorProgress(const QString &text, QWidget *parent,
                                   const char *name)
    : QSemiModal(parent, name, true)
{
    QVBoxLayout *layout = new QVBoxLayout(this, 4);
    label = new QLabel(text, this);
    layout->addWidget(label);
    layout->addSpacing(20);
    progress = new KProgress(0, 100, 0, KProgress::Horizontal, this);
    layout->addWidget(progress);
    layout->addSpacing(20);
    resize(sizeHint());
    show();
};


KIFSingleColorEdit::KIFSingleColorEdit(QWidget *parent, const char *name)
    :QDialog(parent, name, true)
{
    QGridLayout *layout = new QGridLayout(this, 1, 1, 4);
    colorList = new QListBox(this);
    connect(colorList, SIGNAL(selected(QListBoxItem *)), this,
            SLOT(slotSelected(QListBoxItem *)));

    QImage *image = startImage();

    // create a sorted list of RGB entries
    KIFColorProgress *progress =
        new KIFColorProgress(i18n("Finding all color values..."), this);
    QApplication::setOverrideCursor(waitCursor);
    int current;
    unsigned int rgb;
    KIFRgbList rgbList;
    rgbList.setAutoDelete(true);
    unsigned int *data = (unsigned int *)image->bits();
    int percent=0, tmp;
    int total = image->width()*image->height();
    for(current=0; current < total; ++current){
        rgb = data[current];
        if(rgbList.find(&rgb) == -1){
            unsigned int *newItem = new unsigned int;
            *newItem = rgb;
            //rgbList.inSort(newItem);
            rgbList.append(newItem);
        }
        tmp = (int)(current/(float)total*100);
        if(tmp != percent){
            percent = tmp;
            progress->setValue(percent);
        }
        kapp->processEvents();
    }
    progress->setLabel(i18n("Sorting colors..."));
    kapp->processEvents();
    rgbList.sort();

    // insert sorted list entries in listbox
    QListIterator<unsigned int> it(rgbList);
    QColor c;
    QString cName;
    for(; it.current(); ++it){
        c.setRgb(*(it.current()));
        cName.sprintf("#%.2x%.2x%.2x", c.red(), c.green(), c.blue());
        (void)new KIFColorListItem(colorList, cName, c);
    }
    delete progress;
    QApplication::restoreOverrideCursor();

    // done with list creation

    layout->addMultiCellWidget(colorList, 0, 1, 0, 0);

    // Color selector
    colorSelect = new ColorSelector(this);
    connect(colorSelect, SIGNAL(rgbChanged(int, int, int)), this,
            SLOT(slotRGBChanged(int, int, int)));

    layout->addMultiCellWidget(colorSelect, 0, 0, 1, 2);

    // Color select button
    colorBtn = new KColorButton(this);
    connect(colorBtn, SIGNAL(changed(const QColor &)), this,
            SLOT(slotColorBtnChanged(const QColor &)));
    layout->addWidget(colorBtn, 1, 1);

    // Sep and dlg buttons
    layout->addMultiCellWidget(new KSeparator(this), 2, 2, 0, 2);
    KButtonBox *bbox = new KButtonBox(this);
    connect(bbox->addButton(i18n("Revert")), SIGNAL(clicked()), this,
            SLOT(slotRevert()));
    bbox->addStretch();
    connect(bbox->addButton(i18n("Done")), SIGNAL(clicked()), this,
            SLOT(accept()));
    bbox->setMinimumSize(bbox->sizeHint());
    layout->addMultiCellWidget(bbox, 3, 3, 0, 2);
    layout->setColStretch(0, 1);
    layout->setRowStretch(0, 1);

    colorList->setCurrentItem(0);
    slotSelected(colorList->item(0));
    resize(sizeHint());
    setCaption(i18n("Individual Colors"));
}

void KIFSingleColorEdit::slotRevert()
{
    ;
}

void KIFSingleColorEdit::slotRGBChanged(int r, int g, int b)
{
    QColor c(r, g, b);
    colorBtn->setColor(c);
    replaceCurrent(c);
}

void KIFSingleColorEdit::slotColorBtnChanged(const QColor &c)
{
    colorSelect->slotSetColor(c);
    replaceCurrent(c);
}

void KIFSingleColorEdit::slotSelected(QListBoxItem *i)
{
    QColor c(((KIFColorListItem *)i)->color());
    colorSelect->slotSetColor(c);
    colorBtn->setColor(c);
}

void KIFSingleColorEdit::replaceCurrent(const QColor &newColor)
{
    kdWarning() << "In SingleColorEdit::replaceCurrent" << endl;
    KIFColorListItem *item = (KIFColorListItem *)colorList->
        item(colorList->currentItem());
    unsigned int oldRgb = item->color().rgb();
    unsigned int newRgb = newColor.rgb();

    QImage *image = startImage();

    unsigned int *data = (unsigned int *)image->bits();
    int current, total = image->width()*image->height();
    for(current=0; current < total; ++current){
        if(data[current] == oldRgb){
            data[current] = newRgb;
        }
    }
    item->setColor(newColor);
    colorList->triggerUpdate(false);
    updateImageWindow();
}

    
    

#include "singlecolors.moc"

