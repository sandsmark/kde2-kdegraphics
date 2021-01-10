#include "widgets.h"
#include <qpainter.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <klocale.h>

ColorWidget::ColorWidget(const QString &title, const QString &item1,
                         const QString &item2, const QString &item3, int min1,
                         int max1, int min2, int max2, int min3, int max3,
                         QWidget *parent, const char *name)
    : QGroupBox(title, parent, name)
{
    QVBoxLayout *layout = new QVBoxLayout(this, 4);
    layout->addSpacing(14);
    i1Input = new KIntNumInput(0, this);
    i1Input->setLabel(item1);
    i1Input->setRange(min1, max1);
    connect(i1Input, SIGNAL(valueChanged(int)), this,
            SLOT(slotItemChanged(int)));
    layout->addWidget(i1Input);
    i2Input = new KIntNumInput(i1Input, 0, this);
    i2Input->setLabel(item2);
    i2Input->setRange(min2, max2);
    connect(i2Input, SIGNAL(valueChanged(int)), this,
            SLOT(slotItemChanged(int)));
    layout->addWidget(i2Input);
    i3Input = new KIntNumInput(i2Input, 0, this);
    i3Input->setLabel(item3);
    i3Input->setRange(min3, max3);
    connect(i3Input, SIGNAL(valueChanged(int)), this,
            SLOT(slotItemChanged(int)));
    layout->addWidget(i3Input);
    layout->addStretch(1);
}

void ColorWidget::slotSetItems(int val1, int val2, int val3)
{
    i1Input->setValue(val1);
    i2Input->setValue(val2);
    i3Input->setValue(val3);
}

void ColorWidget::slotItemChanged(int)
{
    emit valueChanged(i1Input->value(), i2Input->value(), i3Input->value());
}

RGBSelectWidget::RGBSelectWidget(int startR, int startG, int startB,
                                 QWidget *parent, const char *name)
    : ColorWidget(i18n("RGB Value"), i18n("Red"), i18n("Green"), i18n("Blue"),
                  0, 255, 0, 255, 0, 255, parent, name)
{
    slotSetItems(startR, startG, startB);
}

HSVSelectWidget::HSVSelectWidget(int startH, int startS, int startV,
                                 QWidget *parent, const char *name)
    : ColorWidget(i18n("HSV Value"), i18n("Hue"), i18n("Saturation"), i18n("Value"),
                  0, 359, 0, 255, 0, 255, parent, name)
{
    slotSetItems(startH, startS, startV);
}

ColorSelector::ColorSelector(QWidget *parent, const char *name)
    : QWidget(parent, name)
{
    QVBoxLayout *layout = new QVBoxLayout(this, 4);
    rgbWidget = new RGBSelectWidget(0, 0, 0, this);
    connect(rgbWidget, SIGNAL(valueChanged(int, int, int)), this,
            SLOT(slotRGBChange(int, int, int)));
    layout->addWidget(rgbWidget);
    hsvWidget = new HSVSelectWidget(0, 0, 0, this);
    connect(hsvWidget, SIGNAL(valueChanged(int, int, int)), this,
            SLOT(slotHSVChange(int, int, int)));
    layout->addWidget(hsvWidget);
    layout->addStretch(1);
}

void ColorSelector::slotSetRGB(int r, int g, int b)
{
    rgbWidget->slotSetItems(r, g, b);
    QColor c(r, g, b);
    int h, s, v;
    c.hsv(&h, &s, &v);
    hsvWidget->slotSetItems(h, s, v);
}

void ColorSelector::slotSetColor(const QColor &c)
{
    rgbWidget->slotSetItems(c.red(), c.green(), c.blue());
    int h, s, v;
    c.hsv(&h, &s, &v);
    hsvWidget->slotSetItems(h, s, v);
}

void ColorSelector::slotRGBChange(int r, int g, int b)
{
    QColor c(r, g, b);
    int h, s, v;
    c.hsv(&h, &s, &v);
    hsvWidget->slotSetItems(h, s, v);
    emit colorChanged(c);
    emit rgbChanged(r, g, b);
}

void ColorSelector::slotHSVChange(int h, int s, int v)
{
    QColor c;
    c.setHsv(h, s, v);
    rgbWidget->slotSetItems(c.red(), c.green(), c.blue());
    emit colorChanged(c);
    emit rgbChanged(c.red(), c.green(), c.blue());
}


RGBSlider::RGBSlider(int min, int max, int pageStep, int val, Orientation o,
                     QWidget *parent, const char *name)
    : QSlider(min, max, pageStep, val, o, parent, name)
{
    emitSig = true;
    connect(this, SIGNAL(valueChanged(int)), this,
            SLOT(slotSliderChanged(int)));
}


void RGBSlider::setSneakyValue(int val)
{
    emitSig = false;
    setValue(val);
    emitSig = true;
}

void RGBSlider::slotSliderChanged(int val)
{
    if(emitSig)
        emit(userValueChanged(val));
}

RGBColorButton::RGBColorButton(QWidget *parent, const char *name)
    : QButton(parent, name)
{
    setFixedSize(20, 20);
    QRegion maskRegion(QRect(1, 1, 18, 18));
    maskRegion += QRect(1, 0, 18, 1);
    maskRegion += QRect(1, 19, 18, 1);
    maskRegion += QRect(0, 1, 1, 18);
    maskRegion += QRect(19, 1, 1, 18);
    setMask(maskRegion);
}

void RGBColorButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(Qt::black);
    QRect r = rect();
    p.drawRect(r);
    bool sunken = isOn() || isDown();
    /*
    qDrawShadePanel(&p, r.x()+1, r.y()+1, r.width()-2, r.height()-2,
                    colorGroup(), sunken, 1, sunken ?
                    &colorGroup().brush(QColorGroup::Mid) :
                    &colorGroup().brush(QColorGroup::Button));*/
    p.fillRect(r.x()+1, r.y()+1, r.width()-2, r.height()-2,
               sunken ? colorGroup().brush(QColorGroup::Button) :
               colorGroup().brush(QColorGroup::Midlight));
    p.setPen(colorGroup().light());
    if(!sunken){
        p.drawLine(r.x()+1, r.y()+1, r.right()-1, r.y()+1);
        p.drawLine(r.x()+1, r.y()+1, r.x()+1, r.bottom()-1);
    }
    else{
        p.drawLine(r.right()-1, r.y()+1, r.right()-1, r.bottom()-1);
        p.drawLine(r.x()+1, r.bottom()-1, r.right()-1, r.bottom()-1);
    }
    if(pixmap())
        p.drawPixmap((width()-pixmap()->width())/2,
                     (height()-pixmap()->height())/2, *pixmap());
}

void RGBEditPreview::paintEvent(QPaintEvent *ev)
{
    QRect r = ev->rect();
    bitBlt(this, r.topLeft(), pix, r, Qt::CopyROP);
}

#include "widgets.moc"


