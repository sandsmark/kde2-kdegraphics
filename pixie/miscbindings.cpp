#include "miscbindings.h"
#include "imagewindow.h"
#include "imagelist.h"
#include "ifapp.h"

extern KIFImageWindow *pluginHackWinPtr;
extern KIFImageList *imageList;

void showProgress()
{
    kifapp()->startBusyCursor();
    kifapp()->processEvents();
}

void incProgress()
{
    kifapp()->incBusyCursor();
        kifapp()->processEvents();
}

void updateImage()
{
    pluginHackWinPtr->slotUpdateFromImage();
}


void pluginSetURL(const QString &fileStr)
{
    imageList->slotAddAndSetURL(fileStr);
}

QImage* currentImage()
{
    return(pluginHackWinPtr->image());
}

