#ifndef __WIDGETS_H
#define __WIDGETS_H

#include <qslider.h>
#include <qbutton.h>
#include <qpixmap.h>
#include <knuminput.h>
#include <qgroupbox.h>

// base class for RGB and HSV selection widgets
class ColorWidget : public QGroupBox
{
    Q_OBJECT
public:
    ColorWidget(const QString &title, const QString &item1,
                const QString &item2, const QString &item3, int min1, int max1,
                int min2, int max2, int min3, int max3, QWidget *parent=0,
                const char *name=0);
public slots:
    void slotSetItems(int val1, int val2, int val3);
signals:
    void valueChanged(int val1, int val2, int val3);
protected slots:
    void slotItemChanged(int val);
protected:
    KIntNumInput *i1Input, *i2Input, *i3Input;
};

// RGB selection widget
class RGBSelectWidget : public ColorWidget
{
public:
    RGBSelectWidget(int startR, int startG, int startB, QWidget *parent=0,
                    const char *name=0);
};

// HSV selection widget
class HSVSelectWidget : public ColorWidget
{
public:
    HSVSelectWidget(int startH, int startS, int startV, QWidget *parent=0,
                    const char *name=0);
};

// Both HSV and RGB color selector
class ColorSelector : public QWidget
{
    Q_OBJECT
public:
    ColorSelector(QWidget *parent=0, const char *name=0);
public slots:
    void slotSetRGB(int r, int g, int b);
    void slotSetColor(const QColor &c);
signals:
    void colorChanged(const QColor &c);
    void rgbChanged(int r, int g, int b);
protected slots:
    void slotRGBChange(int r, int g, int b);
    void slotHSVChange(int h, int s, int v);
protected:
    RGBSelectWidget *rgbWidget;
    HSVSelectWidget *hsvWidget;
};

// Image RGB channel slider (-255 to 255 with inc and dec)
class RGBSlider : public QSlider
{
    Q_OBJECT
public:
    RGBSlider(int min, int max, int pageStep, int val, Orientation o,
              QWidget *parent, const char *name=0);
    void setSneakyValue(int val);
public slots:
    void slotIncrement(){setValue(value()+1); emit sliderReleased();}
    void slotDecrement(){setValue(value()-1); emit sliderReleased();}
    void slotReset(){setValue(0); emit sliderReleased();}
protected slots:
    void slotSliderChanged(int value);
signals:
    void userValueChanged(int val);
protected:
    bool emitSig;
};

// Image RGB channel inc or dec base button
class RGBColorButton : public QButton
{
public:
    RGBColorButton(QWidget *parent=0,const char *name=0);
protected:
    void paintEvent(QPaintEvent *ev);
};

// little something to bitBlt a pixmap (faster than QLabel)
class RGBEditPreview : public QWidget
{
public:
    RGBEditPreview(QPixmap *pixmap, QWidget *parent=0, const char *name=0)
        : QWidget(parent, name){pix = pixmap; setFixedSize(pix->size());}
protected:
    void paintEvent(QPaintEvent *ev);

    QPixmap *pix;
};

#endif

