#include "widgets.h"
#include "dualimagergb.h"
#include "channeledit.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qpixmap.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kstddirs.h>


RGBDualChannelWidget::RGBDualChannelWidget(KIFImageColorDialog *parent,
                                   const char *name)
    : QWidget(parent, name)
{
    QVBoxLayout *layout = new QVBoxLayout(this, 4);

    QPixmap incPix(KGlobal::dirs()->findResource("appdata", "inc2.png"));
    QPixmap decPix(KGlobal::dirs()->findResource("appdata", "dec2.png"));
    QPixmap resetPix(SmallIcon("reload"));
    
    // RGB
    QGroupBox *rgbBox = new QGroupBox(i18n("Standard RGB"), this);
    QGridLayout *rgbLayout = new QGridLayout(rgbBox, 1, 1, 4);
    QLabel *lbl = new QLabel(i18n("Red Channel:"), rgbBox);
    rgbLayout->addRowSpacing(0, 14);
    rgbLayout->addMultiCellWidget(lbl, 1, 1, 0, 2);
    rSlider = new RGBSlider(-255, 255, 5, 0, Horizontal, rgbBox);
    connect(rSlider, SIGNAL(userValueChanged(int)), this,
            SLOT(slotRGBDragged(int)));
    connect(rSlider, SIGNAL(sliderReleased()), this,
            SLOT(slotRGBChanged()));
    RGBColorButton *btn = new RGBColorButton(rgbBox);
    btn->setPixmap(resetPix);
    connect(btn, SIGNAL(clicked()), rSlider, SLOT(slotReset()));
    rgbLayout->addWidget(btn, 2, 0);
    btn = new RGBColorButton(rgbBox);
    btn->setPixmap(decPix);
    connect(btn, SIGNAL(clicked()), rSlider, SLOT(slotDecrement()));
    rgbLayout->addWidget(btn, 2, 1);
    rgbLayout->addWidget(rSlider, 2, 2);
    btn = new RGBColorButton(rgbBox);
    btn->setPixmap(incPix);
    connect(btn, SIGNAL(clicked()), rSlider, SLOT(slotIncrement()));
    rgbLayout->addWidget(btn, 2, 3);

    lbl = new QLabel(i18n("Green Channel:"), rgbBox);
    rgbLayout->addMultiCellWidget(lbl, 3, 3, 0, 2);
    gSlider = new RGBSlider(-255, 255, 5, 0, Horizontal, rgbBox);
    connect(gSlider, SIGNAL(userValueChanged(int)), this,
            SLOT(slotRGBDragged(int)));
    connect(gSlider, SIGNAL(sliderReleased()), this,
            SLOT(slotRGBChanged()));
    btn = new RGBColorButton(rgbBox);
    btn->setPixmap(resetPix);
    connect(btn, SIGNAL(clicked()), gSlider, SLOT(slotReset()));
    rgbLayout->addWidget(btn, 4, 0);
    btn = new RGBColorButton(rgbBox);
    btn->setPixmap(decPix);
    connect(btn, SIGNAL(clicked()), gSlider, SLOT(slotDecrement()));
    rgbLayout->addWidget(btn, 4, 1);
    rgbLayout->addWidget(gSlider, 4, 2);
    btn = new RGBColorButton(rgbBox);
    btn->setPixmap(incPix);
    connect(btn, SIGNAL(clicked()), gSlider, SLOT(slotIncrement()));
    rgbLayout->addWidget(btn, 4, 3);

    lbl = new QLabel(i18n("Blue Channel:"), rgbBox);
    rgbLayout->addMultiCellWidget(lbl, 5, 5, 0, 2);
    bSlider = new RGBSlider(-255, 255, 5, 0, Horizontal, rgbBox);
    connect(bSlider, SIGNAL(userValueChanged(int)), this,
            SLOT(slotRGBDragged(int)));
    connect(bSlider, SIGNAL(sliderReleased()), this,
            SLOT(slotRGBChanged()));
    btn = new RGBColorButton(rgbBox);
    btn->setPixmap(resetPix);
    connect(btn, SIGNAL(clicked()), bSlider, SLOT(slotReset()));
    rgbLayout->addWidget(btn, 6, 0);
    btn = new RGBColorButton(rgbBox);
    btn->setPixmap(decPix);
    connect(btn, SIGNAL(clicked()), bSlider, SLOT(slotDecrement()));
    rgbLayout->addWidget(btn, 6, 1);
    rgbLayout->addWidget(bSlider, 6, 2);
    btn = new RGBColorButton(rgbBox);
    btn->setPixmap(incPix);
    connect(btn, SIGNAL(clicked()), bSlider, SLOT(slotIncrement()));
    rgbLayout->addWidget(btn, 6, 3);
    rgbLayout->setColStretch(2, 1);
    layout->addWidget(rgbBox);

    // Dual
    QGroupBox *dualBox = new QGroupBox(i18n("Dual RGB"), this);
    QGridLayout *dualLayout = new QGridLayout(dualBox, 1, 1, 4);
    lbl = new QLabel(i18n("Yellow:"), dualBox);
    dualLayout->addRowSpacing(0, 14);
    dualLayout->addMultiCellWidget(lbl, 1, 1, 0, 2);
    ySlider = new RGBSlider(-255, 255, 5, 0, Horizontal, dualBox);
    connect(ySlider, SIGNAL(userValueChanged(int)), this,
            SLOT(slotRGBDragged(int)));
    connect(ySlider, SIGNAL(sliderReleased()), this,
            SLOT(slotRGBChanged()));
    btn = new RGBColorButton(dualBox);
    btn->setPixmap(resetPix);
    connect(btn, SIGNAL(clicked()), ySlider, SLOT(slotReset()));
    dualLayout->addWidget(btn, 2, 0);
    btn = new RGBColorButton(dualBox);
    btn->setPixmap(decPix);
    connect(btn, SIGNAL(clicked()), ySlider, SLOT(slotDecrement()));
    dualLayout->addWidget(btn, 2, 1);
    dualLayout->addWidget(ySlider, 2, 2);
    btn = new RGBColorButton(dualBox);
    btn->setPixmap(incPix);
    connect(btn, SIGNAL(clicked()), ySlider, SLOT(slotIncrement()));
    dualLayout->addWidget(btn, 2, 3);

    lbl = new QLabel(i18n("Cyan:"), dualBox);
    dualLayout->addMultiCellWidget(lbl, 3, 3, 0, 2);
    cSlider = new RGBSlider(-255, 255, 5, 0, Horizontal, dualBox);
    connect(cSlider, SIGNAL(userValueChanged(int)), this,
            SLOT(slotRGBDragged(int)));
    connect(cSlider, SIGNAL(sliderReleased()), this,
            SLOT(slotRGBChanged()));
    btn = new RGBColorButton(dualBox);
    btn->setPixmap(resetPix);
    connect(btn, SIGNAL(clicked()), cSlider, SLOT(slotReset()));
    dualLayout->addWidget(btn, 4, 0);
    btn = new RGBColorButton(dualBox);
    btn->setPixmap(decPix);
    connect(btn, SIGNAL(clicked()), cSlider, SLOT(slotDecrement()));
    dualLayout->addWidget(btn, 4, 1);
    dualLayout->addWidget(cSlider, 4, 2);
    btn = new RGBColorButton(dualBox);
    btn->setPixmap(incPix);
    connect(btn, SIGNAL(clicked()), cSlider, SLOT(slotIncrement()));
    dualLayout->addWidget(btn, 4, 3);

    lbl = new QLabel(i18n("Magenta:"), dualBox);
    dualLayout->addMultiCellWidget(lbl, 5, 5, 0, 2);
    mSlider = new RGBSlider(-255, 255, 5, 0, Horizontal, dualBox);
    connect(mSlider, SIGNAL(userValueChanged(int)), this,
            SLOT(slotRGBDragged(int)));
    connect(mSlider, SIGNAL(sliderReleased()), this,
            SLOT(slotRGBChanged()));
    btn = new RGBColorButton(dualBox);
    btn->setPixmap(resetPix);
    connect(btn, SIGNAL(clicked()), mSlider, SLOT(slotReset()));
    dualLayout->addWidget(btn, 6, 0);
    btn = new RGBColorButton(dualBox);
    btn->setPixmap(decPix);
    connect(btn, SIGNAL(clicked()), mSlider, SLOT(slotDecrement()));
    dualLayout->addWidget(btn, 6, 1);
    dualLayout->addWidget(mSlider, 6, 2);
    btn = new RGBColorButton(dualBox);
    btn->setPixmap(incPix);
    connect(btn, SIGNAL(clicked()), mSlider, SLOT(slotIncrement()));
    dualLayout->addWidget(btn, 6, 3);
    dualLayout->setColStretch(2, 1);
    layout->addWidget(dualBox);
}

void RGBDualChannelWidget::rgb(int &r, int &g, int &b)
{
    r = rSlider->value() + ySlider->value() + mSlider->value();
    g = gSlider->value() + ySlider->value() + cSlider->value();
    b = bSlider->value() + cSlider->value() + mSlider->value();
}

void RGBDualChannelWidget::reset()
{
    rSlider->setSneakyValue(0);
    gSlider->setSneakyValue(0);
    bSlider->setSneakyValue(0);
    ySlider->setSneakyValue(0);
    cSlider->setSneakyValue(0);
    mSlider->setSneakyValue(0);
}

void RGBDualChannelWidget::slotRGBChanged()
{
    emit updateThumb();
    emit updateImage();
}

void RGBDualChannelWidget::slotRGBDragged(int)
{
    emit updateThumb();
}



#include "dualimagergb.moc"


