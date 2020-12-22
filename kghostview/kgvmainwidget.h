#ifndef KGVMAINWIDGET_H
#define KGVMAINWIDGET_H

#include <qwidget.h>

class KGVMainWidget : public QWidget
{
    Q_OBJECT
    
public:
    KGVMainWidget( QWidget* parent = 0, const char* name = 0 );

signals:
    void spacePressed();

protected:
    virtual void keyPressEvent( QKeyEvent* );
};

#endif
