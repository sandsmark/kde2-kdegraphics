#include "init.h"
#include "channeledit.h"
#include "imagehsv.h"
#include "dualimagergb.h"
#include "widgets.h"
#include <qpainter.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <kbuttonbox.h>
#include <klocale.h>
#include <kseparator.h>
#include <kdebug.h>

KIFImageColorDialog::KIFImageColorDialog(QWidget *parent, const char *name)
    : QDialog(parent, name, true)
{

    QHBoxLayout *layout = new QHBoxLayout(this, 4);

    QTabWidget *tabBox = new QTabWidget(this);

    dualWidget = new RGBDualChannelWidget(this);
    connect(dualWidget, SIGNAL(updateThumb()), this,
            SLOT(slotUpdateThumb()));
    connect(dualWidget, SIGNAL(updateImage()), this,
            SLOT(slotUpdateImage()));
    tabBox->addTab(dualWidget, i18n("Dual RGB"));

    hsvWidget = new HSVChannelWidget(this);
    connect(hsvWidget, SIGNAL(updateThumb()), this,
            SLOT(slotUpdateThumb()));
    connect(hsvWidget, SIGNAL(updateImage()), this,
            SLOT(slotUpdateImage()));

    tabBox->addTab(hsvWidget, i18n("HSV Channels"));

    layout->addWidget(tabBox);
    QVBoxLayout *ctrlLayout = new QVBoxLayout(4); // controls on the right
    layout->addLayout(ctrlLayout);
    // Preview
    QImage *image = startImage();
    if(image->width() > image->height()){
        float percent = (((float)78)/image->width());
        int h = (int)(image->height()*percent);
        origPreview = image->smoothScale(78, h);
    }
    else{
        float percent = (((float)78)/image->height());
        int w = (int)(image->width()*percent);
        origPreview = image->smoothScale(w, 78);
    }
    currentPreview.create(origPreview.width(), origPreview.height(),
                          origPreview.depth());
    origImage = *startImage();
    origImage.detach();
    previewPix.convertFromImage(origPreview);
    QGroupBox *previewBox = new QGroupBox(i18n("Preview"), this);
    QGridLayout *previewLayout = new QGridLayout(previewBox, 1, 1, 4);
    previewLayout->addRowSpacing(0, 14);
    preview = new RGBEditPreview(&previewPix, previewBox);
    previewLayout->addWidget(preview, 1, 0);
    previewLayout->setRowStretch(2, 1);
    previewLayout->setColStretch(1, 1);
    ctrlLayout->addWidget(previewBox, 0, 1);
    
    ctrlLayout->addStretch(1);

    QPushButton *pshBtn = new QPushButton(i18n("Brighten"), this);
    ctrlLayout->addWidget(pshBtn);
    pshBtn = new QPushButton(i18n("Dim"), this);
    ctrlLayout->addWidget(pshBtn);
    ctrlLayout->addSpacing(20);
    pshBtn = new QPushButton(i18n("Revert"), this);
    connect(pshBtn, SIGNAL(clicked()), this, SLOT(slotRevert()));
    ctrlLayout->addWidget(pshBtn);
    pshBtn = new QPushButton(i18n("Done"), this);
    connect(pshBtn, SIGNAL(clicked()), this, SLOT(accept()));
    ctrlLayout->addWidget(pshBtn);
    
    resize(sizeHint());
    setCaption(i18n("Image Color"));
}

void KIFImageColorDialog::slotRevert()
{
    dualWidget->reset();
    hsvWidget->reset();
    changeImage(true);
    changeImage(false);
}

void KIFImageColorDialog::slotUpdateThumb()
{
    changeImage(true);
}

void KIFImageColorDialog::slotUpdateImage()
{
    changeImage(false);
}

void KIFImageColorDialog::changeImage(bool thumbnail)
{
    QImage *destImage = startImage();
    int current, total;
    int r, g, b, rOff, gOff, bOff;
    int h, s, v, hOff, sOff, vOff;
    unsigned int *srcdata, *destdata;

    if(thumbnail){
        total = origPreview.width()*origPreview.height();
        srcdata = (unsigned int *)origPreview.bits();
        destdata = (unsigned int *)currentPreview.bits();

    }
    else{
        total = origImage.width()*origImage.height();
        srcdata = (unsigned int *)origImage.bits();
        destdata = (unsigned int *)destImage->bits();
    }
    
    dualWidget->rgb(rOff, gOff, bOff);
    // get HSV values and see if we need to use 'em
    hsvWidget->hsv(hOff, sOff, vOff);
    if(hOff != 0 || sOff != 0 || vOff != 0){ // keep HSV check outside loop
        for(current=0; current < total; ++current){
            r = qRed(srcdata[current]) + rOff; // first do RGB
            g = qGreen(srcdata[current]) + gOff;
            b = qBlue(srcdata[current]) + bOff;
            if(r > 255) r = 255;
            if(g > 255) g = 255;
            if(b > 255) b = 255;
            if(r < 0) r = 0;
            if(g < 0) g = 0;
            if(b < 0) b = 0;
            convertRGB2HSV(r, g, b, h, s, v); // go to HSV and modifiy
            h += hOff;
            s += sOff;
            v += vOff;
            if(h > 359) h = 359;
            if(s > 255) s = 255;
            if(v > 255) v = 255;
            if(h < 0) h = 0;
            if(s < 0) s = 0;
            if(v < 0) v = 0;
            convertHSV2RGB(h, s, v, r, g, b); // go back to RGB and set
            destdata[current] = qRgb(r, g, b);
        }
    }
    else{ // No HSV mods, we can just do RGB
        for(current=0; current < total; ++current){
            r = qRed(srcdata[current]) + rOff;
            g = qGreen(srcdata[current]) + gOff;
            b = qBlue(srcdata[current]) + bOff;
            if(r > 255) r = 255;
            if(g > 255) g = 255;
            if(b > 255) b = 255;
            if(r < 0) r = 0;
            if(g < 0) g = 0;
            if(b < 0) b = 0;
            destdata[current] = qRgb(r, g, b);
        }
    }
    if(thumbnail){
        previewPix.convertFromImage(currentPreview);
        preview->repaint(false);
    }
    else
        updateImageWindow();
}


// this and convertRGB2HSV is strongly based on Qt's QColor HSV handling.
void KIFImageColorDialog::convertHSV2RGB(int h, int s, int v,
                                         int &r, int &g, int &b)
{
    if ( h < -1 || s > 255 || v > 255 ) {
        kdWarning() << "Pixie: invalid HSV value: " << h << ", " << s << ", " << v << endl;
        r = 0;
        g = 0;
        b = 0;
        return;
    }
    r = v;
    g = v;
    b = v;
    if (!s || h == -1)
	;
    else {
        if (h >= 360)
            h %= 360;
        unsigned int f = h%60;
        h /= 60;
        unsigned int p = (unsigned int)(2*v*(255-s)+255)/510;
        unsigned int q, t;
	if (h&1) {
            q = (unsigned int)(2*v*(15300-s*f)+15300)/30600;
            switch(h){
            case 1:
                r=(int)q; g=(int)v, b=(int)p;
                break;
            case 3:
                r=(int)p; g=(int)q, b=(int)v;
                break;
            case 5:
                r=(int)v; g=(int)p, b=(int)q;
                break;
	    }
        }
        else {
            t = (unsigned int)(2*v*(15300-(s*(60-f)))+15300)/30600;
	    switch(h){
            case 0:
                r=(int)v; g=(int)t, b=(int)p;
                break;
            case 2:
                r=(int)p; g=(int)v, b=(int)t;
                break;
            case 4:
                r=(int)t; g=(int)p, b=(int)v;
                break;
            }
        }
    }
}

// this and convertHSV2RGB is strongly based on Qt's QColor HSV handling.
void KIFImageColorDialog::convertRGB2HSV(int r, int g, int b,
                                         int &h, int &s, int &v)
{
    unsigned int max = r;	       
    int maxcomponent = 0;
    if ((unsigned int)g > max) {
	max = g;
	maxcomponent = 1;
    }
    if ((unsigned int)b > max) {
	max = b;
	maxcomponent = 2;
    }
    unsigned int min = r;	      
    if ((unsigned int)g < min)
        min = g;
    if ((unsigned int)b < min)
        min = b;
    int delta = max-min;
    v = max;
    s = max ? (510*delta+max)/(2*max) : 0;
    if(!s)
	h = -1;
    else {
        switch(maxcomponent){
        case 0:
            if (g >= b)
                h = (120*(g-b)+delta)/(2*delta);
            else
                h = (120*(g-b+delta)+delta)/(2*delta) + 300;
            break;
        case 1:
            if (b > r)
                h = 120 + (120*(b-r)+delta)/(2*delta);
            else
                h = 60 + (120*(b-r+delta)+delta)/(2*delta);
            break;
        case 2:
            if (r > g)
                h = 240 + (120*(r-g)+delta)/(2*delta);
            else
                h = 180 + (120*(r-g+delta)+delta)/(2*delta);
            break;
        }
    }
}

#include "channeledit.moc"

