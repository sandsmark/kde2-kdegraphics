#include "dialogs.h"
#include <qimage.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qslider.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <kimageeffect.h>
#include <kseparator.h>

GradientDialog::GradientDialog(QWidget *parent, const char *name)
    : QDialog(parent, name, true)
{
    QColor hColor(Qt::black);
    QColor lColor(Qt::white);
    effectID = KPixmapEffect::VerticalGradient;

    QHBoxLayout *layout = new QHBoxLayout(this, 4);
    QVBoxLayout *colorLayout = new QVBoxLayout(4);
    layout->addLayout(colorLayout);

    // rgb stuff
    QGroupBox *highGrp = new QGroupBox(i18n("High Color"), this);
    QGridLayout *hLayout = new QGridLayout(highGrp, 1, 1, 4);
    hLayout->addRowSpacing(0, 14);
    hBtn = new KColorButton(hColor, highGrp);
    connect(hBtn, SIGNAL(changed(const QColor &)), this,
            SLOT(slotHColorBtn(const QColor &)));
    hLayout->addWidget(hBtn, 1, 0);
    rHInput = new KIntNumInput(hColor.red(), highGrp);
    rHInput->setRange(0, 255);
    rHInput->setLabel(i18n("Red"));
    connect(rHInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotHInputChanged(int)));
    hLayout->addMultiCellWidget(rHInput, 2, 2, 0, 1);
    gHInput = new KIntNumInput(rHInput, hColor.green(), highGrp);
    gHInput->setRange(0, 255);
    gHInput->setLabel(i18n("Green"));
    connect(gHInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotHInputChanged(int)));
    hLayout->addMultiCellWidget(gHInput, 3, 3, 0, 1);
    bHInput = new KIntNumInput(gHInput, hColor.blue(), highGrp);
    bHInput->setRange(0, 255);
    bHInput->setLabel(i18n("Blue"));
    connect(bHInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotHInputChanged(int)));
    hLayout->addMultiCellWidget(bHInput, 4, 4, 0, 1);
    colorLayout->addWidget(highGrp);

    QGroupBox *lowGrp = new QGroupBox(i18n("Low Color"), this);
    QGridLayout *lLayout = new QGridLayout(lowGrp, 1, 1, 4);
    lLayout->addRowSpacing(0, 14);
    lBtn = new KColorButton(lColor, lowGrp);
    connect(lBtn, SIGNAL(changed(const QColor &)), this,
            SLOT(slotLColorBtn(const QColor &)));
    lLayout->addWidget(lBtn, 1, 0);
    rLInput = new KIntNumInput(lColor.red(), lowGrp);
    rLInput->setRange(0, 255);
    rLInput->setLabel(i18n("Red"));
    connect(rLInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotLInputChanged(int)));
    lLayout->addMultiCellWidget(rLInput, 2, 2, 0, 1);
    gLInput = new KIntNumInput(rLInput, lColor.green(), lowGrp);
    gLInput->setRange(0, 255);
    gLInput->setLabel(i18n("Green"));
    connect(gLInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotLInputChanged(int)));
    lLayout->addMultiCellWidget(gLInput, 3, 3, 0, 1);
    bLInput = new KIntNumInput(gLInput, lColor.blue(), lowGrp);
    bLInput->setRange(0, 255);
    bLInput->setLabel(i18n("Blue"));
    connect(bLInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotLInputChanged(int)));
    lLayout->addMultiCellWidget(bLInput, 4, 4, 0, 1);
    colorLayout->addWidget(lowGrp);
    colorLayout->addStretch(1);

    // controls
    QVBoxLayout *ctrlLayout = new QVBoxLayout(4);
    layout->addLayout(ctrlLayout);

    QLabel *lbl = new QLabel(i18n("Preview:"), this);
    ctrlLayout->addWidget(lbl);

    gradientLbl = new QLabel(this);
    gradientLbl->setMinimumSize(58, 58);
    gradientLbl->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    ctrlLayout->addWidget(gradientLbl);

    lbl = new QLabel(i18n("Type:"), this);
    ctrlLayout->addWidget(lbl);

    QComboBox *effCombo = new QComboBox(this);
    connect(effCombo, SIGNAL(activated(int)), this,
            SLOT(slotType(int)));
    effCombo->insertItem(i18n("Vertical"), 0);
    effCombo->insertItem(i18n("Horizontal"), 1);
    effCombo->insertItem(i18n("Diagonal"), 2);
    effCombo->insertItem(i18n("CrossDiagonal"), 3);
    effCombo->insertItem(i18n("Pyramid"), 4);
    effCombo->insertItem(i18n("Rectangle"), 5);
    effCombo->insertItem(i18n("PipeCross"), 6);
    effCombo->insertItem(i18n("Elliptic"), 7);
    ctrlLayout->addWidget(effCombo);

    ctrlLayout->addStretch(1);

    QPushButton *btn = new QPushButton(i18n("Cancel"), this);
    connect(btn, SIGNAL(clicked()), this, SLOT(reject()));
    ctrlLayout->addWidget(btn);
    btn = new QPushButton(i18n("OK"), this);
    connect(btn, SIGNAL(clicked()), this, SLOT(accept()));
    ctrlLayout->addWidget(btn);

    resize(sizeHint());
    updatePreview();
};

void GradientDialog::updatePreview()
{

    if(previewPix.isNull())
        previewPix.resize(58, 58);
    KPixmapEffect::gradient(previewPix, hBtn->color(), lBtn->color(),
                            effectID);
    gradientLbl->setPixmap(previewPix);
}

void GradientDialog::slotType(int val)
{
    switch(val){
    case 1:
        effectID = KPixmapEffect::HorizontalGradient;
        break;
    case 2:
        effectID = KPixmapEffect::DiagonalGradient;
        break;
    case 3:
        effectID = KPixmapEffect::CrossDiagonalGradient;
        break;
    case 4:
        effectID = KPixmapEffect::PyramidGradient;
        break;
    case 5:
        effectID = KPixmapEffect::RectangleGradient;
        break;
    case 6:
        effectID = KPixmapEffect::PipeCrossGradient;
        break;
    case 7:
        effectID = KPixmapEffect::EllipticGradient;
        break;
    case 0:
    default:
        effectID = KPixmapEffect::VerticalGradient;
        break;
    }
    updatePreview();
}

void GradientDialog::slotHColorBtn(const QColor &c)
{
    rHInput->setValue(c.red());
    gHInput->setValue(c.green());
    bHInput->setValue(c.blue());
    updatePreview();
}

void GradientDialog::slotLColorBtn(const QColor &c)
{
    rLInput->setValue(c.red());
    gLInput->setValue(c.green());
    bLInput->setValue(c.blue());
    updatePreview();
}

void GradientDialog::slotHInputChanged(int)
{
    QColor c(rHInput->value(), gHInput->value(), bHInput->value());
    hBtn->setColor(c);
    updatePreview();
}

void GradientDialog::slotLInputChanged(int)
{
    QColor c(rLInput->value(), gLInput->value(), bLInput->value());
    lBtn->setColor(c);
    updatePreview();
}

UnbalancedDialog::UnbalancedDialog(QWidget *parent, const char *name)
    : QDialog(parent, name, true)
{
    QColor hColor(Qt::black);
    QColor lColor(Qt::white);
    effectID = KPixmapEffect::VerticalGradient;

    QHBoxLayout *layout = new QHBoxLayout(this, 4);
    QVBoxLayout *colorLayout = new QVBoxLayout(4);
    layout->addLayout(colorLayout);

    // rgb stuff
    QGroupBox *highGrp = new QGroupBox(i18n("High Color"), this);
    QGridLayout *hLayout = new QGridLayout(highGrp, 1, 1, 4);
    hLayout->addRowSpacing(0, 14);
    hBtn = new KColorButton(hColor, highGrp);
    connect(hBtn, SIGNAL(changed(const QColor &)), this,
            SLOT(slotHColorBtn(const QColor &)));
    hLayout->addWidget(hBtn, 1, 0);
    rHInput = new KIntNumInput(hColor.red(), highGrp);
    rHInput->setLabel(i18n("Red"));
    rHInput->setRange(0, 255);
    connect(rHInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotHInputChanged(int)));
    hLayout->addMultiCellWidget(rHInput, 2, 2, 0, 1);
    gHInput = new KIntNumInput(rHInput, hColor.green(), highGrp);
    gHInput->setLabel(i18n("Green"));
    gHInput->setRange(0, 255);
    connect(gHInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotHInputChanged(int)));
    hLayout->addMultiCellWidget(gHInput, 3, 3, 0, 1);
    bHInput = new KIntNumInput(gHInput, hColor.blue(), highGrp);
    bHInput->setRange(0, 255);
    bHInput->setLabel(i18n("Blue"));
    connect(bHInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotHInputChanged(int)));
    hLayout->addMultiCellWidget(bHInput, 4, 4, 0, 1);
    colorLayout->addWidget(highGrp);

    QGroupBox *lowGrp = new QGroupBox(i18n("Low Color"), this);
    QGridLayout *lLayout = new QGridLayout(lowGrp, 1, 1, 4);
    lLayout->addRowSpacing(0, 14);
    lBtn = new KColorButton(lColor, lowGrp);
    connect(lBtn, SIGNAL(changed(const QColor &)), this,
            SLOT(slotLColorBtn(const QColor &)));
    lLayout->addWidget(lBtn, 1, 0);
    rLInput = new KIntNumInput(lColor.red(), lowGrp);
    rLInput->setRange(0, 255);
    rLInput->setLabel(i18n("Red"));
    connect(rLInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotLInputChanged(int)));
    lLayout->addMultiCellWidget(rLInput, 2, 2, 0, 1);
    gLInput = new KIntNumInput(rLInput, lColor.green(), lowGrp);
    gLInput->setRange(0, 255);
    gLInput->setLabel(i18n("Green"));
    connect(gLInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotLInputChanged(int)));
    lLayout->addMultiCellWidget(gLInput, 3, 3, 0, 1);
    bLInput = new KIntNumInput(gLInput, lColor.blue(), lowGrp);
    bLInput->setRange(0, 255);
    bLInput->setLabel(i18n("Blue"));
    connect(bLInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotLInputChanged(int)));
    lLayout->addMultiCellWidget(bLInput, 4, 4, 0, 1);
    colorLayout->addWidget(lowGrp);

    xInput = new KIntNumInput(100, this);
    xInput->setRange(-200, 200);
    xInput->setLabel(i18n("X Decay"));
    connect(xInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotXInputChanged(int)));
    colorLayout->addWidget(xInput);
    yInput = new KIntNumInput(100, this);
    yInput->setRange(-200, 200);
    yInput->setLabel(i18n("Y Decay"));
    connect(yInput, SIGNAL(valueChanged(int)), this,
            SLOT(slotYInputChanged(int)));
    colorLayout->addWidget(yInput);

    colorLayout->addStretch(1);

    // controls
    QVBoxLayout *ctrlLayout = new QVBoxLayout(4);
    layout->addLayout(ctrlLayout);

    QLabel *lbl = new QLabel(i18n("Preview:"), this);
    ctrlLayout->addWidget(lbl);

    gradientLbl = new QLabel(this);
    gradientLbl->setMinimumSize(58, 58);
    gradientLbl->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    ctrlLayout->addWidget(gradientLbl);

    lbl = new QLabel(i18n("Type:"), this);
    ctrlLayout->addWidget(lbl);

    QComboBox *effCombo = new QComboBox(this);
    connect(effCombo, SIGNAL(activated(int)), this,
            SLOT(slotType(int)));
    effCombo->insertItem(i18n("Vertical"), 0);
    effCombo->insertItem(i18n("Horizontal"), 1);
    effCombo->insertItem(i18n("Diagonal"), 2);
    effCombo->insertItem(i18n("CrossDiagonal"), 3);
    effCombo->insertItem(i18n("Pyramid"), 4);
    effCombo->insertItem(i18n("Rectangle"), 5);
    effCombo->insertItem(i18n("PipeCross"), 6);
    effCombo->insertItem(i18n("Elliptic"), 7);
    ctrlLayout->addWidget(effCombo);

    ctrlLayout->addStretch(1);

    QPushButton *btn = new QPushButton(i18n("Cancel"), this);
    connect(btn, SIGNAL(clicked()), this, SLOT(reject()));
    ctrlLayout->addWidget(btn);
    btn = new QPushButton(i18n("OK"), this);
    connect(btn, SIGNAL(clicked()), this, SLOT(accept()));
    ctrlLayout->addWidget(btn);

    resize(sizeHint());
    updatePreview();
};

void UnbalancedDialog::updatePreview()
{

    if(previewPix.isNull())
        previewPix.resize(58, 58);
    KPixmapEffect::unbalancedGradient(previewPix, hBtn->color(), lBtn->color(),
                                      effectID, xInput->value(),
                                      yInput->value());
    gradientLbl->setPixmap(previewPix);
}

void UnbalancedDialog::slotType(int val)
{
    switch(val){
    case 1:
        effectID = KPixmapEffect::HorizontalGradient;
        break;
    case 2:
        effectID = KPixmapEffect::DiagonalGradient;
        break;
    case 3:
        effectID = KPixmapEffect::CrossDiagonalGradient;
        break;
    case 4:
        effectID = KPixmapEffect::PyramidGradient;
        break;
    case 5:
        effectID = KPixmapEffect::RectangleGradient;
        break;
    case 6:
        effectID = KPixmapEffect::PipeCrossGradient;
        break;
    case 7:
        effectID = KPixmapEffect::EllipticGradient;
        break;
    case 0:
    default:
        effectID = KPixmapEffect::VerticalGradient;
        break;
    }
    updatePreview();
}

void UnbalancedDialog::slotHColorBtn(const QColor &c)
{
    rHInput->setValue(c.red());
    gHInput->setValue(c.green());
    bHInput->setValue(c.blue());
    updatePreview();
}

void UnbalancedDialog::slotLColorBtn(const QColor &c)
{
    rLInput->setValue(c.red());
    gLInput->setValue(c.green());
    bLInput->setValue(c.blue());
    updatePreview();
}

void UnbalancedDialog::slotHInputChanged(int)
{
    QColor c(rHInput->value(), gHInput->value(), bHInput->value());
    hBtn->setColor(c);
    updatePreview();
}

void UnbalancedDialog::slotLInputChanged(int)
{
    QColor c(rLInput->value(), gLInput->value(), bLInput->value());
    lBtn->setColor(c);
    updatePreview();
}

void UnbalancedDialog::slotXInputChanged(int)
{
    updatePreview();
}

void UnbalancedDialog::slotYInputChanged(int)
{
    updatePreview();
}







#include "dialogs.moc"


