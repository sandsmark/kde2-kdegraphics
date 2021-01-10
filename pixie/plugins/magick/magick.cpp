#include "dialog.h"
#include <ifplugin.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <qimage.h>
#include <klocale.h>
#include <kapp.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <bindings.h>
#include <mini-magick/magick.h>

// span never seems to get set properly, I am not sure why. 
// David: I turned long long into off_t to match monitor.h. It's not used anyway.
extern "C" void runMagickProgress(const char * /*text*/, const off_t /*quantum*/,
                                  const off_t span)
{
    kdWarning() << "KDE span: " << (long) span << endl;
    KIFPlugin::incAniCursor();
}

// we try to avoid using ImageMagick supplied strings since they are not
// translated.
void runMagickErrorDlg(const ErrorType err, const char*, const char *)
{
    QString errorStr(i18n("ImageMagick Plugin Error:\n\n"));
    switch(err){
    case ResourceLimitError:
        errorStr += i18n("Operation exceeds resource limits.");
        break;
    case XServerError:
        errorStr += i18n("X11 window system error.");
        break;
    case OptionError:
        errorStr += i18n("Invalid option specified.");
        break;
    case DelegateError:
        errorStr += i18n("ImageMagick delegate subroutine error.");
        break;
    case MissingDelegateError:
        errorStr += i18n("Missing ImageMagick delegate subroutine.");
        break;
    case CorruptImageError:
        errorStr += i18n("The image has been corrupted.");
        break;
    case FileOpenError:
        errorStr += i18n("Unable to open specified file.");
        break;
    case BlobError:
        errorStr += i18n("ImageMagick BLOB error.");
        break;
    case CacheError:
        errorStr += i18n("ImageMagick cache error.");
        break;
    case UndefinedError:
    default:
        errorStr += i18n("Unknown ImageMagick error.");
        break;
    }
    KMessageBox::error(KIFPlugin::parent(), errorStr,
                       i18n("Pixie Error"));
}

void runMagickWarningDlg(const WarningType warn, const char *, const char *)
{
    QString errorStr(i18n("ImageMagick Plugin Warning:\n\n"));
    switch(warn){
    case ResourceLimitWarning:
        errorStr += i18n("Operation exceeds resource limits.");
        break;
    case XServerWarning:
        errorStr += i18n("X11 window system error.");
        break;
    case OptionWarning:
        errorStr += i18n("Invalid option specified.");
        break;
    case DelegateWarning:
        errorStr += i18n("ImageMagick delegate subroutine error.");
        break;
    case MissingDelegateWarning:
        errorStr += i18n("Missing ImageMagick delegate subroutine.");
        break;
    case CorruptImageWarning:
        errorStr += i18n("The image has been corrupted.");
        break;
    case FileOpenWarning:
        errorStr += i18n("Unable to open specified file.");
        break;
    case BlobWarning:
        errorStr += i18n("ImageMagick BLOB error.");
        break;
    case CacheWarning:
        errorStr += i18n("ImageMagick cache error.");
        break;
    case UndefinedWarning:
    default:
        errorStr += i18n("Unknown ImageMagick error.");
        break;
    }
    KMessageBox::sorry(KIFPlugin::parent(), errorStr,
                       i18n("Pixie Warning"));
}

void KIFBlur()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    MagickIntegerDlg dlg(i18n("Please select the blur weight."),
                         i18n("Percent (0-100%)"), 0, 100, 20,
                         KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());

        Image *newImage = BlurImage(magickImage, dlg.value());
        GetPixels(newImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(newImage);
        DestroyImage(magickImage);
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
    }
}

void KIFImplode()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    MagickIntegerDlg dlg(i18n("Please select the implode extent."),
                         i18n("Percent (0-100%)"), 0, 100, 20,
                         KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());

        Image *newImage = ImplodeImage(magickImage, dlg.value());
        GetPixels(newImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(newImage);
        DestroyImage(magickImage);
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
    }
}

void KIFEdge()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    MagickIntegerDlg dlg(i18n("Please select the edge detect weight."),
                         i18n("Percent (0-100%)"), 0, 100, 20,
                         KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        Image *newImage = EdgeImage(magickImage, dlg.value());
        GetPixels(newImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(newImage);
        DestroyImage(magickImage);
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
    }
}

void KIFSharpen()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    MagickIntegerDlg dlg(i18n("Please select the sharpening weight."),
                         i18n("Percent (0-100%)"), 0, 100, 20,
                         KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        Image *newImage = SharpenImage(magickImage, dlg.value());
        GetPixels(newImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(newImage);
        DestroyImage(magickImage);
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
    }
}

void KIFSwirl()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    MagickIntegerDlg dlg(i18n("Please select the degrees to swirl."),
                         i18n("Degrees (0-360)"), 0, 360, 45,
                         KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        Image *newImage = SwirlImage(magickImage, dlg.value());
        GetPixels(newImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(newImage);
        DestroyImage(magickImage);
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
    }
}

void KIFMedianFilter()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    MagickIntegerDlg dlg(i18n("Please select the filter radius."),
                         i18n("Radius(0-8)"), 0, 8, 3,
                         KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        Image *newImage = MedianFilterImage(magickImage,
                                            (unsigned int)dlg.value());
        GetPixels(newImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(newImage);
        DestroyImage(magickImage);
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
    }

}

// color is f*cked up :P Not needed anyways because channel editing
/*
void KIFColorize()
{
    MagickColorIntegerDlg dlg(i18n("Please select the color to fill and opacity"),
                              i18n("Opacity(1-100)"), 1, 100, 50,
                              i18n("Color:"),
                              KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        showProgress();
        QColor color(dlg.color());
        QString colorStr;
        colorStr.sprintf("#%02x%02x%02x", color.red(), color.green(),
                         color.blue());
        colorStr = colorStr.upper();
        kdWarning() <<  "Color: " << colorStr.latin1() << endl;
        QString opacity;
        opacity = opacity.setNum(dlg.value());
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        Image *newImage = ColorizeImage(magickImage, opacity.latin1(),
                                        colorStr.latin1());
        GetPixels(newImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(newImage);
        DestroyImage(magickImage);
        KIFPlugin::updateImage();
    }
}*/

void KIFOilPainting()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    MagickIntegerDlg dlg(i18n("Please select the filter radius."),
                         i18n("Radius(0-8)"), 0, 8, 3,
                         KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        Image *newImage = OilPaintImage(magickImage,
                                        (unsigned int)dlg.value());
        GetPixels(newImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(newImage);
        DestroyImage(magickImage);
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
    }
}

void KIFRoll()
{
    QImage *image = KIFPlugin::image();
    MagickDblIntegerDlg dlg(i18n("Please select the offsets to roll image:"),
                            i18n("Horizontal"), -(image->width()), image->width(), 0,
                            i18n("Vertical"), -(image->height()), image->height(), 0,
                            KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        Image *newImage = RollImage(magickImage, dlg.value1(), dlg.value2());
        GetPixels(newImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(newImage);
        DestroyImage(magickImage);
        KIFPlugin::updateImage();
    }
}

void KIFWave()
{
    QImage *image = KIFPlugin::image();
    MagickDblIntegerDlg dlg(i18n("Please select sine wave parameters:"),
                            i18n("Amplitude"), 1, 100, 25,
                            i18n("Frequency"), 1, 500, 150,
                            KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        Image *newImage = WaveImage(magickImage, dlg.value1(), dlg.value2());
        image->reset();
        image->create(newImage->columns, newImage->rows, 32);
        GetPixels(newImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(newImage);
        DestroyImage(magickImage);
        KIFPlugin::updateImage();
    }
}

// works on orig
void KIFThreshold()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    MagickIntegerDlg dlg(i18n("Please select the threshold value."),
                         i18n("Value (0-256)"), 0, 256, 128,
                         KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        ThresholdImage(magickImage, dlg.value());
        GetPixels(magickImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(magickImage);
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
    }
}

// operates on orig
void KIFSolarize()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    MagickIntegerDlg dlg(i18n("Please select the extent to solarize."),
                         i18n("Factor (0-360)"), 0, 360, 45,
                         KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        SolarizeImage(magickImage, dlg.value());
        GetPixels(magickImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(magickImage);
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
    }
}

void KIFNoise()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    MagickComboDlg dlg(i18n("Please select the type of noise."),
                       i18n("Type:"), KIFPlugin::parent());
    dlg.insert(i18n("Gaussian"), 0);
    dlg.insert(i18n("Gaussian Multiply"), 1);
    dlg.insert(i18n("Impulse"), 2);
    dlg.insert(i18n("Laplacian"), 3);
    dlg.insert(i18n("Poisson"), 4);
    
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        NoiseType n;
        switch(dlg.value()){
        case 1:
            n = MultiplicativeGaussianNoise;
            break;
        case 2:
            n = ImpulseNoise;
            break;
        case 3:
            n = LaplacianNoise;
            break;
        case 4:
            n = PoissonNoise;
            break;
        case 0:
        default:
            n = GaussianNoise;
            break;
        }
        Image *newImage = AddNoiseImage(magickImage, n);
        GetPixels(newImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(newImage);
        DestroyImage(magickImage);
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
    }
}

void KIFDespeckle()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    KIFPlugin::startAniCursor();
    Image *magickImage = CreateImage(image->width(), image->height(),
                                     "RGBA", (StorageType)0, image->bits());
    Image *newImage = DespeckleImage(magickImage);
    GetPixels(newImage, 0, 0, image->width(), image->height(),
              "RGBA", (StorageType)0, image->bits());
    DestroyImage(newImage);
    DestroyImage(magickImage);
    if(KIFPlugin::selection()->isValid())
        KIFPlugin::putSelection(selection);
    KIFPlugin::updateImage();
}

// works on orig
void KIFEqualize()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    KIFPlugin::startAniCursor();
    Image *magickImage = CreateImage(image->width(), image->height(),
                                     "RGBA", (StorageType)0,
                                     image->bits());
    EqualizeImage(magickImage);
    GetPixels(magickImage, 0, 0, image->width(), image->height(),
              "RGBA", (StorageType)0, image->bits());
    DestroyImage(magickImage);
    if(KIFPlugin::selection()->isValid())
        KIFPlugin::putSelection(selection);
    KIFPlugin::updateImage();
}

// works on orig
void KIFNormalize()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    KIFPlugin::startAniCursor();
    Image *magickImage = CreateImage(image->width(), image->height(),
                                     "RGBA", (StorageType)0,
                                     image->bits());
    NormalizeImage(magickImage);
    GetPixels(magickImage, 0, 0, image->width(), image->height(),
              "RGBA", (StorageType)0, image->bits());
    DestroyImage(magickImage);
    if(KIFPlugin::selection()->isValid())
        KIFPlugin::putSelection(selection);
    KIFPlugin::updateImage();
}

// works on orig
void KIFGamma()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    MagickDoubleDlg dlg(i18n("Please select the gamma correction value"),
                        i18n("Value (0.0-15.0)"), 0.0, 15.0, 1.0,
                        KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        QString val;
        val.sprintf("%f", dlg.value());
        kdWarning() << "Using " << val.latin1() << endl;
        GammaImage(magickImage, val.latin1());
        GetPixels(magickImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(magickImage);
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
    }
}

// works on orig
void KIFModulate()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    MagickDoubleDlg dlg(i18n("Please select the modulation percent."),
                        i18n("Percent (0.0-100.0)"), 0.0, 100.00, 33.0,
                        KIFPlugin::parent());
    if(dlg.exec() == QDialog::Accepted){
        KIFPlugin::startAniCursor();
        Image *magickImage = CreateImage(image->width(), image->height(),
                                         "RGBA", (StorageType)0,
                                         image->bits());
        QString val;
        val.sprintf("%f", dlg.value());
        kdWarning() << "Using " << val.latin1() << endl;
        ModulateImage(magickImage, val.latin1());
        GetPixels(magickImage, 0, 0, image->width(), image->height(),
                  "RGBA", (StorageType)0, image->bits());
        DestroyImage(magickImage);
        if(KIFPlugin::selection()->isValid())
            KIFPlugin::putSelection(selection);
        KIFPlugin::updateImage();
    }
}

void KIFEmboss()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    KIFPlugin::startAniCursor();
    Image *magickImage = CreateImage(image->width(), image->height(),
                                     "RGBA", (StorageType)0, image->bits());
    Image *newImage = EmbossImage(magickImage);
    GetPixels(newImage, 0, 0, image->width(), image->height(),
              "RGBA", (StorageType)0, image->bits());
    DestroyImage(newImage);
    DestroyImage(magickImage);
    if(KIFPlugin::selection()->isValid())
        KIFPlugin::putSelection(selection);
    KIFPlugin::updateImage();
}

void KIFReduceNoise()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    KIFPlugin::startAniCursor();
    Image *magickImage = CreateImage(image->width(), image->height(),
                                     "RGBA", (StorageType)0, image->bits());
    Image *newImage = ReduceNoiseImage(magickImage);
    GetPixels(newImage, 0, 0, image->width(), image->height(),
              "RGBA", (StorageType)0, image->bits());
    DestroyImage(newImage);
    DestroyImage(magickImage);
    if(KIFPlugin::selection()->isValid())
        KIFPlugin::putSelection(selection);
    KIFPlugin::updateImage();
}

// this has problems
void KIFEnhance()
{
    QImage *image;
    QImage selection;
    if(KIFPlugin::selection()->isValid()){
        selection = KIFPlugin::copySelection();
        image = &selection;
    }
    else
        image = KIFPlugin::image();
    KIFPlugin::startAniCursor();
    Image *magickImage = CreateImage(image->width(), image->height(),
                                     "RGBA", (StorageType)0, image->bits());
    Image *newImage = EnhanceImage(magickImage);
    GetPixels(newImage, 0, 0, image->width(), image->height(),
              "RGBA", (StorageType)0, image->bits());
    DestroyImage(newImage);
    DestroyImage(magickImage);
    if(KIFPlugin::selection()->isValid())
        KIFPlugin::putSelection(selection);
    KIFPlugin::updateImage();
}

extern "C" void process()
{
    QString effectStr = KIFPlugin::effectName();
    kdWarning() << "ImageMagick plugin called for " << effectStr.latin1() << endl;

    // tell imagemagick what routines to run for messages
    MonitorHandler oldMonitor;
    oldMonitor = SetMonitorHandler(runMagickProgress);
    
    ErrorHandler oldError;
    oldError = SetErrorHandler(runMagickErrorDlg);

    WarningHandler oldWarning;
    oldWarning = SetWarningHandler(runMagickWarningDlg);
        
    // some effect
    if(effectStr == "Blur")
        KIFBlur();
    if(effectStr == "Reduce Noise")
        KIFReduceNoise();
    else if(effectStr == "Despeckle")
        KIFDespeckle();
    else if(effectStr == "Edge Detect")
        KIFEdge();
    else if(effectStr == "Emboss")
        KIFEmboss();
    else if(effectStr == "Enhance")
        KIFEnhance();
    else if(effectStr == "Implode")
        KIFImplode();
    else if(effectStr == "Sharpen")
        KIFSharpen();
    else if(effectStr == "Swirl")
        KIFSwirl();
    else if(effectStr == "Solarize")
        KIFSolarize();
    else if(effectStr == "Threshold")
        KIFThreshold();
    else if(effectStr == "Noise")
        KIFNoise();
    else if(effectStr == "Median Filter")
        KIFMedianFilter();
    else if(effectStr == "Oil Painting")
        KIFOilPainting();
    else if(effectStr == "Equalize")
        KIFEqualize();
    else if(effectStr == "Normalize")
        KIFNormalize();
    else if(effectStr == "Roll")
        KIFRoll();
    else if(effectStr == "Wave")
        KIFWave();
    else if(effectStr == "Gamma Correction")
        KIFGamma();
    else if(effectStr == "Modulate")
        KIFModulate();

    // set old handlers
    SetMonitorHandler(oldMonitor);
    SetErrorHandler(oldError);
    SetWarningHandler(oldWarning);
}


