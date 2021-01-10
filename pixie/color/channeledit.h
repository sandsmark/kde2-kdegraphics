#ifndef __KIF_RGBCHANNELS_H
#define __KIF_RGBCHANNELS_H

#include <qdialog.h>
#include <qimage.h>
#include <qpixmap.h>

class HSVChannelWidget;
class RGBDualChannelWidget;
class RGBEditPreview;

class KIFImageColorDialog : public QDialog
{
    Q_OBJECT
public:
    KIFImageColorDialog(QWidget *parent=0, const char *name=0);
public slots:
    void slotUpdateThumb();
    void slotUpdateImage();
    void slotRevert();
protected:
    void convertHSV2RGB(int h, int s, int v, int &r, int &g, int &b);
    void convertRGB2HSV(int r, int g, int b, int &h, int &s, int &v);
    void changeImage(bool thumbnail);

    HSVChannelWidget *hsvWidget;
    RGBDualChannelWidget *dualWidget;
    RGBEditPreview *preview;
    QImage origPreview, currentPreview, origImage;
    QPixmap previewPix;
};


#endif
