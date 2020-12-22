#ifndef KGVPAGEDECORATOR_H
#define KGVPAGEDECORATOR_H

#include <qhbox.h>

class KGVPageDecorator : public QHBox
{
public:
    KGVPageDecorator( QWidget* parent = 0, const char* name = 0 );
    ~KGVPageDecorator() { ; }

    /**
     * Reimplemented from QObject to let mouse events from child widgets
     * appear to come from this widget.
     */
    bool eventFilter( QObject*, QEvent* );

protected:
    /**
     * Reimplemented from QObject to automatically insert an event filter
     * on child widgets.
     */
    virtual void childEvent( QChildEvent* );

    virtual void drawFrame( QPainter* );
    virtual void drawFrameMask( QPainter* );

private:
    unsigned int _margin;
    unsigned int _borderWidth;
    QPoint _shadowOffset;
};

#endif
