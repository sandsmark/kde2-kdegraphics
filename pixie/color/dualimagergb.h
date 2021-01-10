#ifndef __KIF_DUALRGBCHANNEL_H
#define __KIF_DUALRGBCHANNEL_H

#include <qwidget.h>

class RGBSlider;
class KIFImageColorDialog;

class RGBDualChannelWidget : public QWidget
{
    Q_OBJECT
public:
    RGBDualChannelWidget(KIFImageColorDialog *parent, const char *name=0);
    // -255 to 255
    void rgb(int &r, int &g, int &b);
    void reset();
protected slots:
    void slotRGBChanged();
    void slotRGBDragged(int val);
signals:
    void updateThumb();
    void updateImage();
protected:
    
    RGBSlider *rSlider, *gSlider, *bSlider, *ySlider, *cSlider, *mSlider;
};


#endif
