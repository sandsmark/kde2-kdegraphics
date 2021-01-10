#include "dialogs.h"
#include <qapplication.h>
#include <qcursor.h>
#include <klocale.h>
#include <kimageeffect.h>
#include <kcolordlg.h>
#include <qimage.h>
#include <qpixmap.h>

#include <ifplugin.h>

void solidFill()
{
    QImage *image;
    QImage selection;
    QColor c;
    if(KColorDialog::getColor(c, KIFPlugin::parent())){
        if(KIFPlugin::selection()->isValid()){
            selection = KIFPlugin::copySelection();
            image = &selection;
        }
        else
            image = KIFPlugin::image();
        QApplication::setOverrideCursor(Qt::waitCursor);
        image->fill(c.rgb());
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
        QApplication::restoreOverrideCursor();
    }
}

void mirrorHorizontal()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    QApplication::setOverrideCursor(Qt::waitCursor);
    *image = image->mirror(true, false);
    if(KIFPlugin::selection()->isValid())
        KIFPlugin::putSelection(selection);
    KIFPlugin::updateImage();
    QApplication::restoreOverrideCursor();
}

void mirrorVertical()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    QApplication::setOverrideCursor(Qt::waitCursor);
    *image = image->mirror();
    if(KIFPlugin::selection()->isValid())
        KIFPlugin::putSelection(selection);
    KIFPlugin::updateImage();
    QApplication::restoreOverrideCursor();
}

void gradient()
{
    GradientDialog dlg(KIFPlugin::parent());
    dlg.setCaption(i18n("Gradient"));
    if(dlg.exec() == QDialog::Accepted){
        QImage *image;
        QImage selection;
        if(KIFPlugin::selection()->isValid()){
            selection = KIFPlugin::copySelection();
            image = &selection;
        }
        else
            image = KIFPlugin::image();
        QApplication::setOverrideCursor(Qt::waitCursor);
        *image = KImageEffect::gradient(image->size(), dlg.highColor(),
                                        dlg.lowColor(),
                                        (KImageEffect::GradientType)dlg.grType());
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
        QApplication::restoreOverrideCursor();
    }
}

void unbalancedGradient()
{
    UnbalancedDialog dlg(KIFPlugin::parent());
    dlg.setCaption(i18n("Gradient"));
    if(dlg.exec() == QDialog::Accepted){
        QImage *image;
        QImage selection;
        if(KIFPlugin::selection()->isValid()){
            selection = KIFPlugin::copySelection();
            image = &selection;
        }
        else
            image = KIFPlugin::image();
        QApplication::setOverrideCursor(Qt::waitCursor);
        *image =
            KImageEffect::unbalancedGradient(image->size(), dlg.highColor(),
                                             dlg.lowColor(),
                                             (KImageEffect::GradientType)dlg.grType(),
                                             dlg.xDecay(), dlg.yDecay());
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
        QApplication::restoreOverrideCursor();
    }
}

void grayscale()
{
    int startx, starty, endx, endy;
    QRect *selRect = KIFPlugin::selection();
    QImage *image = KIFPlugin::image();
    if(selRect->isValid()){
        startx  = selRect->x();
        endx = selRect->right();
        starty = selRect->y();
        endy = selRect->bottom();
    }
    else{
        startx = 0, starty = 0;
        endx = image->width()-1;
        endy = image->height()-1;
    }
    
    QApplication::setOverrideCursor(Qt::waitCursor);
    unsigned int *data;
    int x, y, pixel;
    for(y=starty; y <= endy; ++y){
        data = (unsigned int *)image->scanLine(y);
        for(x=startx; x <= endx; ++x){
            pixel = qGray(data[x]);
            data[x] = qRgb(pixel, pixel, pixel);
        }
    }
    KIFPlugin::updateImage();
    QApplication::restoreOverrideCursor();
}


extern "C" void process()
{
    QString effectStr = KIFPlugin::effectName();
    qWarning("Plugin routine called for %s", effectStr.latin1());
    if(effectStr == "Horizontal Mirror")
        mirrorHorizontal();
    else if(effectStr == "Vertical Mirror")
        mirrorVertical();
    else if(effectStr == "Gradient")
        gradient();
    else if(effectStr == "Grayscale")
        grayscale();
    else if(effectStr == "Solid Fill")
        solidFill();
    else if(effectStr == "Unbalanced Gradient")
        unbalancedGradient();
}



