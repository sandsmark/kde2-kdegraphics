#ifndef __KIF_THUMBPROPS_H
#define __KIF_THUMBPROPS_H

#include <qdialog.h>

class KIFThumbProperties : public QDialog
{
public:
    KIFThumbProperties(const QPixmap &pixmap, const QString &fileStr,
                       QWidget *parent=0, const char *name=0);
};

#endif

