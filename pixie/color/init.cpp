#include "init.h"
#include "singlecolors.h"
#include "channeledit.h"

extern "C" QImage *(*startImage)() = NULL;
extern "C" void *(*updateImageWindow)() = NULL;

extern "C" {
    void init(QImage *((getStartImage)()), void *((getUpdate)()))
    {
        if(!startImage){
            startImage = getStartImage;
            updateImageWindow = getUpdate;
        }
    }

    void runSingleColor()
    {
        KIFSingleColorEdit dlg;
        dlg.exec();
    }

    void runImageColor()
    {
        KIFImageColorDialog dlg;
        dlg.exec();
    }
}


