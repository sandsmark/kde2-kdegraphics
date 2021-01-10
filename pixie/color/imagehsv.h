#ifndef __KIF_HSVCHANNEL_H
#define __KIF_HSVCHANNEL_H

#include <qwidget.h>

class RGBSlider;
class KIFImageColorDialog;

class HSVChannelWidget : public QWidget
{
    Q_OBJECT
public:
    HSVChannelWidget(KIFImageColorDialog *parent, const char *name=0);
    void reset();
    // hue: -359 to 359, saturation and value: -255 to 255
    void hsv(int &h, int &s, int &v);
protected slots:
    void slotHSVChanged();
    void slotHSVDragged(int val);
signals:
    void updateThumb();
    void updateImage();
protected:
    RGBSlider *hSlider, *sSlider, *vSlider;
};


#endif
