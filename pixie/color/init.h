#ifndef __KIF_COLORPLUGIN_INIT_H
#define __KIF_COLORPLUGIN_INIT_H

#include <qimage.h>

extern "C" QImage *(*startImage)();
extern "C" void *(*updateImageWindow)();

#endif

