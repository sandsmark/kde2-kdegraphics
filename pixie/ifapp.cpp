#include "ifapp.h"

/*
static const char * busy1_xpm[] = {
"18 18 3 1",
"       c None",
".      c #000000",
"+      c #FFFFFF",
"      ......      ",
"    ..++++++..    ",
"   .++++++++++.   ",
"  ..++++++++++..  ",
" ....++++++++.... ",
" .....++++++..... ",
".......++++.......",
"........++........",
"..................",
"..................",
"........++........",
".......++++.......",
" .....++++++..... ",
" ....++++++++.... ",
"  ..++++++++++..  ",
"   .++++++++++.   ",
"    ..++++++..    ",
"      ......      "};

static const char * busy2_xpm[] = {
"18 18 3 1",
"       c None",
".      c #000000",
"+      c #FFFFFF",
"      ......      ",
"    ..........    ",
"   ............   ",
"  ..............  ",
" .++..........++. ",
" .+++........+++. ",
".+++++......+++++.",
".++++++....++++++.",
".+++++++..+++++++.",
".+++++++..+++++++.",
".++++++....++++++.",
".+++++......+++++.",
" .+++........+++. ",
" .++..........++. ",
"  ..............  ",
"   ............   ",
"    ..........    ",
"      ......      "};
*/

static const char * busy1_xpm[] = {
"32 32 3 1",
" 	c None",
".	c #000000",
"+	c #FFFFFF",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"            .......             ",
"          ..+++++++..           ",
"         .+++++++++++.          ",
"        .+++++++++++++.         ",
"       ...+++++++++++...        ",
"      .....+++++++++.....       ",
"      ......+++++++......       ",
"     ........+++++........      ",
"     .........+++.........      ",
"     ..........+..........      ",
"     .....................      ",
"     ..........+..........      ",
"     .........+++.........      ",
"     ........+++++........      ",
"      ......+++++++......       ",
"      .....+++++++++.....       ",
"       ...+++++++++++...        ",
"        .+++++++++++++.         ",
"         .+++++++++++.          ",
"          ..+++++++..           ",
"            .......             ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                "};

static const char * busy2_xpm[] = {
"32 32 3 1",
" 	c None",
".	c #000000",
"+	c #FFFFFF",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"            .......             ",
"          ...........           ",
"         .............          ",
"        ...............         ",
"       .+.............+.        ",
"      .+++...........+++.       ",
"      .++++.........++++.       ",
"     .++++++.......++++++.      ",
"     .+++++++.....+++++++.      ",
"     .++++++++...++++++++.      ",
"     .+++++++++.+++++++++.      ",
"     .++++++++...++++++++.      ",
"     .+++++++.....+++++++.      ",
"     .++++++.......++++++.      ",
"      .++++.........++++.       ",
"      .+++...........+++.       ",
"       .+.............+.        ",
"        ...............         ",
"         .............          ",
"          ...........           ",
"            .......             ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                "};

static KIFApplication* appPtr = NULL;

KIFApplication* kifapp(){return(appPtr);}

KIFApplication::KIFApplication()
    : KApplication()
{
    appPtr = this;
    QPixmap busy1Pix(busy1_xpm);
    QPixmap busy2Pix(busy2_xpm);

    busy1Cursor = QCursor(busy1Pix);
    busy2Cursor = QCursor(busy2Pix);
    inBusy = false;
}

void KIFApplication::startBusyCursor()
{
    if(!inBusy){
        inBusy = true;
        cursorState = false;
        setOverrideCursor(busy1Cursor);
    }
}

void KIFApplication::incBusyCursor()
{
    if(inBusy){
        cursorState = !cursorState;
        if(cursorState)
            setOverrideCursor(busy2Cursor, true);
        else
            setOverrideCursor(busy1Cursor, true);
    }
}

void KIFApplication::stopBusyCursor()
{
    inBusy = false;
    restoreOverrideCursor();
}

void KIFApplication::hideWindows()
{
    emit hideAllWindows();
}

void KIFApplication::showWindows()
{
    emit showAllWindows();
}

#include "ifapp.moc"


