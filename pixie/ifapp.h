#ifndef __KIF_APPLICATION_H
#define __KIF_APPLICATION_H

#include <kapp.h>

class KIFApplication : public KApplication
{
    Q_OBJECT
public:
    KIFApplication();
    void startBusyCursor();
    void incBusyCursor();
    void stopBusyCursor();
    QCursor& busyCursor1(){return(busy1Cursor);}
    QCursor& busyCursor2(){return(busy2Cursor);}
    void hideWindows();
    void showWindows();
signals:
    void hideAllWindows();
    void showAllWindows();
protected:
    bool cursorState, inBusy;
    QCursor busy1Cursor, busy2Cursor;
};

KIFApplication* kifapp();

#endif

