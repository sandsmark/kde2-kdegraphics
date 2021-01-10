#include "ifplugin.h"
#include <qimage.h>
#include <qpixmap.h>
#include <qstring.h>

extern "C" {
    QImage *img_ptr;
    QRect *sel_ptr;
//    bool *update_ptr;
    QWidget *parent_ptr;
    QString effect_str;
    void *(*showani_ptr)();
    void *(*updateani_ptr)();
    void *(*update_image)();
};

QImage* KIFPlugin::image()
{
    return(img_ptr);
}

QRect* KIFPlugin::selection()
{
    return(sel_ptr);
}

QString KIFPlugin::effectName()
{
    return(effect_str);
}

void KIFPlugin::updateImage()
{
    update_image();
}

QWidget* KIFPlugin::parent()
{
    return(parent_ptr);
}

void KIFPlugin::startAniCursor()
{
    showani_ptr();
}

void KIFPlugin::incAniCursor()
{
    updateani_ptr();
}

QImage KIFPlugin::copySelection()
{
    QImage image;
    QRect *r = sel_ptr;
    if(!r->isValid())
        qWarning("KIFPlugin: copySelection called with no selection!");
    else{
        QImage *origImage = img_ptr;
        image.create(r->width(), r->height(), 32);
        int ix, ox, iy, oy;
        unsigned int *input, *output;
        for(oy=0, iy = r->y(); oy < r->height(); ++iy, ++oy){
            input = (unsigned int *)origImage->scanLine(iy);
            output = (unsigned int *)image.scanLine(oy);
            for(ox=0, ix = r->x(); ox < r->width(); ++ix, ++ox)
                output[ox] = input[ix];
        }
    }
    return(image);
}

void KIFPlugin::putSelection(QImage &image)
{
    QRect *r = sel_ptr;
    if(!r->isValid())
        qWarning("KIFPlugin: putSelection called with no selection!");
    else{
        QImage *origImage = img_ptr;
        int ix, ox, iy, oy;
        unsigned int *input, *output;
        for(iy=0, oy = r->y(); iy < r->height(); ++iy, ++oy){
            input = (unsigned int *)image.scanLine(iy);
            output = (unsigned int *)origImage->scanLine(oy);
            for(ix=0, ox = r->x(); ix < r->width(); ++ix, ++ox)
                output[ox] = input[ix];
        }
    }
    update_image();
}
    

extern "C"{
    void kifinternal_init(QString &effectStr, QWidget *parent, QPixmap *,
                          QImage *image, QRect *rect, void *(showP)(),
                          void *(updateP)(), void *(updateImg)())
    {
        img_ptr = image;
        sel_ptr = rect;
        update_image = updateImg;
        //update_ptr = &updateImage;
        //*update_ptr = false;
        showani_ptr = showP;
        updateani_ptr = updateP;
        effect_str = effectStr;
        parent_ptr = parent;
    }
}


