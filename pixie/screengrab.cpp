#include "screengrab.h"
#include "imagelist.h"
#include "ifapp.h"

#include <qcheckbox.h>
#include <qlayout.h>
#include <qlabel.h>

#include <kbuttonbox.h>
#include <klocale.h>
#include <kimageio.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <kdebug.h>

#include <X11/X.h>
#include <X11/Xlib.h>

KIFScreenGrab::KIFScreenGrab(KIFImageList *list, QWidget *parent,
                             const char *name)
    : QSemiModal(parent, name, true)
{
    QVBoxLayout *layout = new QVBoxLayout(this, 4);
    QLabel *lbl = new QLabel(i18n("This dialog will grab either your desktop or\n\
a single application window. If you grab a\n\
single window your mouse cursor will change\n\
into crosshairs and simply select the window\n\
with your mouse."), this);
    layout->addWidget(lbl);
    desktopCB = new QCheckBox(i18n("Grab the entire desktop."), this);
    desktopCB->setChecked(true);
    layout->addWidget(desktopCB);
    hideCB = new QCheckBox(i18n("Hide all pixie windows."), this);
    hideCB->setChecked(true);
    layout->addWidget(hideCB);
    layout->addStretch(1);
    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch(1);
    connect(bbox->addButton(i18n("Cancel")), SIGNAL(clicked()), this,
            SLOT(slotCancel()));
    connect(bbox->addButton(i18n("OK")), SIGNAL(clicked()), this,
            SLOT(slotStartGrab()));
    layout->addWidget(bbox);
    setCaption(i18n("Screenshot"));
    inSelect = false;

    imageList = list;
    show();

    inLoop = true;
    while(inLoop)
        kapp->processEvents();
};

void KIFScreenGrab::slotStartGrab()
{
    QPixmap pixmap;

    if(desktopCB->isChecked()){
        if(hideCB->isChecked()){
            kifapp()->hideWindows();
            kifapp()->processEvents();
            hide();
        }
        QApplication::syncX();
        pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
    }
    else{
        Display *disp = QApplication::desktop()->x11Display();
        Window rootWin, childWin;
        int rootX, rootY, winX, winY;
        unsigned int mask;

        winSelected = false;
        inSelect = true;
        grabMouse(crossCursor);

        if(hideCB->isChecked()){
            kifapp()->hideWindows();
            kifapp()->processEvents();
            move(-4000, -4000);
            QApplication::syncX();
        }

        while(!winSelected)
            kifapp()->processEvents();
        releaseMouse();
        inSelect = false;
        XQueryPointer(disp, DefaultRootWindow(disp), &rootWin, &childWin,
                      &rootX, &rootY, &winX, &winY, &mask);

        QApplication::flushX();
        if(!childWin){
            kdDebug() << "Pixie: no child window selected, using root" << endl;
            pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
        }
        else
            pixmap = QPixmap::grabWindow(childWin);

    }
    KNotifyClient::beep();

    QImage *image = imageList->window()->image();
    //image->detach();
    *image = pixmap.convertToImage();
    if(image->depth() < 32)
        image->convertDepth(32);
    imageList->window()->slotUpdateFromImage();
    if(hideCB->isChecked()){
        kifapp()->showWindows();
        QApplication::syncX();
    }
    if(pixmap.isNull()){
        KMessageBox::error(this, i18n("Unable to take snapshot!"));
        inLoop = false;
        return;
    }
    KURL u =
        KFileDialog::getSaveURL(QString::null,
                                     KImageIO::pattern(KImageIO::Writing),
                                     NULL);
    QString fileStr = u.path();
    if(!fileStr.isEmpty()){
        if(!pixmap.save(fileStr, KImageIO::type(fileStr).latin1()))
            KMessageBox::error(this, i18n("Unable to save snapshot!"));
        else
            imageList->slotAddAndSetURL(fileStr);
    }
    inLoop = false;
}

void KIFScreenGrab::slotCancel()
{
    inLoop = false;
}

void KIFScreenGrab::mousePressEvent(QMouseEvent *ev)
{
    kdDebug() << "In mousePressEvent" << endl;
    if(inSelect)
        winSelected = true;
    else
        QSemiModal::mousePressEvent(ev);
}

#include "screengrab.moc"



