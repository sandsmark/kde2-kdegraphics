#include "widgets.h"
#include "imagehsv.h"
#include "channeledit.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qpixmap.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kstddirs.h>


HSVChannelWidget::HSVChannelWidget(KIFImageColorDialog *parent,
                                   const char *name)
    : QWidget(parent, name)
{
    QVBoxLayout *layout = new QVBoxLayout(this, 4);

    QPixmap incPix(KGlobal::dirs()->findResource("appdata", "inc2.png"));
    QPixmap decPix(KGlobal::dirs()->findResource("appdata", "dec2.png"));
    QPixmap resetPix(SmallIcon("reload"));
    
    // HSV
    QGroupBox *hsvBox = new QGroupBox(i18n("HSV Settings"), this);
    QGridLayout *hsvLayout = new QGridLayout(hsvBox, 1, 1, 4);
    QLabel *lbl = new QLabel(i18n("Hue:"), hsvBox);
    hsvLayout->addRowSpacing(0, 14);
    hsvLayout->addMultiCellWidget(lbl, 1, 1, 0, 2);
    hSlider = new RGBSlider(-359, 359, 5, 0, Horizontal, hsvBox);
    connect(hSlider, SIGNAL(userValueChanged(int)), this,
            SLOT(slotHSVDragged(int)));
    connect(hSlider, SIGNAL(sliderReleased()), this,
            SLOT(slotHSVChanged()));
    RGBColorButton *btn = new RGBColorButton(hsvBox);
    btn->setPixmap(resetPix);
    connect(btn, SIGNAL(clicked()), hSlider, SLOT(slotReset()));
    hsvLayout->addWidget(btn, 2, 0);
    btn = new RGBColorButton(hsvBox);
    btn->setPixmap(decPix);
    connect(btn, SIGNAL(clicked()), hSlider, SLOT(slotDecrement()));
    hsvLayout->addWidget(btn, 2, 1);
    hsvLayout->addWidget(hSlider, 2, 2);
    btn = new RGBColorButton(hsvBox);
    btn->setPixmap(incPix);
    connect(btn, SIGNAL(clicked()), hSlider, SLOT(slotIncrement()));
    hsvLayout->addWidget(btn, 2, 3);

    lbl = new QLabel(i18n("Saturation:"), hsvBox);
    hsvLayout->addMultiCellWidget(lbl, 3, 3, 0, 2);
    sSlider = new RGBSlider(-255, 255, 5, 0, Horizontal, hsvBox);
    connect(sSlider, SIGNAL(userValueChanged(int)), this,
            SLOT(slotHSVDragged(int)));
    connect(sSlider, SIGNAL(sliderReleased()), this,
            SLOT(slotHSVChanged()));
    btn = new RGBColorButton(hsvBox);
    btn->setPixmap(resetPix);
    connect(btn, SIGNAL(clicked()), sSlider, SLOT(slotReset()));
    hsvLayout->addWidget(btn, 4, 0);
    btn = new RGBColorButton(hsvBox);
    btn->setPixmap(decPix);
    connect(btn, SIGNAL(clicked()), sSlider, SLOT(slotDecrement()));
    hsvLayout->addWidget(btn, 4, 1);
    hsvLayout->addWidget(sSlider, 4, 2);
    btn = new RGBColorButton(hsvBox);
    btn->setPixmap(incPix);
    connect(btn, SIGNAL(clicked()), sSlider, SLOT(slotIncrement()));
    hsvLayout->addWidget(btn, 4, 3);

    lbl = new QLabel(i18n("Value:"), hsvBox);
    hsvLayout->addMultiCellWidget(lbl, 5, 5, 0, 2);
    vSlider = new RGBSlider(-255, 255, 5, 0, Horizontal, hsvBox);
    connect(vSlider, SIGNAL(userValueChanged(int)), this,
            SLOT(slotHSVDragged(int)));
    connect(vSlider, SIGNAL(sliderReleased()), this,
            SLOT(slotHSVChanged()));
    btn = new RGBColorButton(hsvBox);
    btn->setPixmap(resetPix);
    connect(btn, SIGNAL(clicked()), vSlider, SLOT(slotReset()));
    hsvLayout->addWidget(btn, 6, 0);
    btn = new RGBColorButton(hsvBox);
    btn->setPixmap(decPix);
    connect(btn, SIGNAL(clicked()), vSlider, SLOT(slotDecrement()));
    hsvLayout->addWidget(btn, 6, 1);
    hsvLayout->addWidget(vSlider, 6, 2);
    btn = new RGBColorButton(hsvBox);
    btn->setPixmap(incPix);
    connect(btn, SIGNAL(clicked()), vSlider, SLOT(slotIncrement()));
    hsvLayout->addWidget(btn, 6, 3);
    hsvLayout->setColStretch(2, 1);
    layout->addWidget(hsvBox);
    layout->addStretch(1);
}

void HSVChannelWidget::hsv(int &h, int &s, int &v)
{
    h = hSlider->value();
    s = sSlider->value();
    v = vSlider->value();
}

void HSVChannelWidget::reset()
{
    hSlider->setSneakyValue(0);
    sSlider->setSneakyValue(0);
    vSlider->setSneakyValue(0);
}

void HSVChannelWidget::slotHSVChanged()
{
    emit updateThumb();
    emit updateImage();
}

void HSVChannelWidget::slotHSVDragged(int)
{
    emit updateThumb();
}


#include "imagehsv.moc"


