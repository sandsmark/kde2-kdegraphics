#ifndef __KDE_DIALOGS
#define __KDE_DIALOGS

#include <qdialog.h>
#include <kcolorbtn.h>
#include <kpixmapeffect.h>
#include <knuminput.h>

class GradientDialog : public QDialog
{
    Q_OBJECT
public:
    GradientDialog(QWidget *parent=0, const char *name=0);
    QColor highColor(){return(hBtn->color());}
    QColor lowColor(){return(lBtn->color());}
    KPixmapEffect::GradientType grType(){return(effectID);}
protected slots:
    void slotHColorBtn(const QColor &c);
    void slotLColorBtn(const QColor &c);
    void slotHInputChanged(int val);
    void slotLInputChanged(int val);
    void slotType(int id);
protected:
    void updatePreview();

    KPixmapEffect::GradientType effectID;
    KIntNumInput *rHInput, *gHInput, *bHInput, *rLInput, *gLInput, *bLInput;
    QLabel *gradientLbl;
    KColorButton *hBtn, *lBtn;
    KPixmap previewPix;
};


// this and GradientDialog should be combined since their virtually
// identical
class UnbalancedDialog : public QDialog
{
    Q_OBJECT
public:
    UnbalancedDialog(QWidget *parent=0, const char *name=0);
    QColor highColor(){return(hBtn->color());}
    QColor lowColor(){return(lBtn->color());}
    int xDecay(){return(xInput->value());}
    int yDecay(){return(yInput->value());}
    KPixmapEffect::GradientType grType(){return(effectID);}
protected slots:
    void slotHColorBtn(const QColor &c);
    void slotLColorBtn(const QColor &c);
    void slotHInputChanged(int val);
    void slotLInputChanged(int val);
    void slotXInputChanged(int val);
    void slotYInputChanged(int val);
    void slotType(int id);
protected:
    void updatePreview();

    KPixmapEffect::GradientType effectID;
    KIntNumInput *rHInput, *gHInput, *bHInput, *rLInput, *gLInput, *bLInput;
    KIntNumInput *xInput, *yInput;
    QLabel *gradientLbl;
    KColorButton *hBtn, *lBtn;
    KPixmap previewPix;
};

/*
class BlendDialog : public QDialog
{
    Q_OBJECT
public:
    BlendDialog(QImage *image, QWidget *parent=0, const char *name=0);
    float intensity();
    QColor background();
    bool antiDir();
    KPixmapEffect::GradientType grType(){return(effectID);}
protected slots:
    void slotIntensityChanged(int val){;}
    void slotColorBtn(const QColor &c){;}
protected:
    bool anti;
    QLabel *previewLbl;
    KPixmap previewPix;
    QImage previewImage;
    QImage origImage;
    KPixmapEffect::GradientType effectID;
    KDoubleNumInput *intensityInput;
    KColorButton *blendColor;
};
*/

#endif

