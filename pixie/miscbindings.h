#ifndef __KIF_MISCBINDINGS_H
#define __KIF_MISCBINDINGS_H

#include <qstring.h>
#include <qimage.h>

// various internal bindings (not for plugin developers)

extern "C"{
    void showProgress();
    void incProgress();
    void updateImage();
    void pluginSetURL(const QString &fileStr);
    QImage *currentImage();
}

#endif

