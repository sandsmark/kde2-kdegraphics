// $Id: spraycan.cpp 75967 2000-12-31 13:23:18Z jcaliff $

#include <kdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <qcursor.h>
#include <qstring.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <klocale.h>
#include "spraycan.h"
#include <kiconloader.h>

SprayCan::SprayCan(const QString & toolname) : Tool(toolname)
{
    drawing= FALSE;
    brushsize= 10;
    density= 100;
    tooltip= i18n("Spray Can");
    props= Tool::HasLineProperties | Tool::HasCustomProperties;

    timer= new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(drawDot()) );
}

SprayCan::~SprayCan()
{
    delete timer;
}

void SprayCan::activating()
{
    kdDebug(4400) << "SprayCan::activating() hook called canvas=" << canvas << "\n" << endl;

    drawing= FALSE;
    canvas->setCursor(crossCursor);
}


void SprayCan::mouseMoveEvent(QMouseEvent *e)
{
    if (isActive() && drawing) 
    {
        x= (e->pos()).x();
        y= (e->pos()).y();
    }
}


void SprayCan::mousePressEvent(QMouseEvent *e)
{
    kdDebug(4400) << "SprayCan::mousePressEvent() handler called\n" << endl;
  
    if (isActive()) 
    {
        if (drawing) 
        {
            kdDebug(4400) << "SprayCan: Warning Left Button press received when pressed\n" << endl;
        }
        x= (e->pos()).x();
        y= (e->pos()).y();
        activeButton= e->button();

        // Start the timer (multishot)
        timer->start(50, FALSE);
        drawing= TRUE;
    }
}


void SprayCan::mouseReleaseEvent(QMouseEvent *e)
{
    kdDebug(4400) << "SprayCan::mouseReleaseEvent() handler called\n" << endl;
  
    if (isActive() && (e->button() == activeButton)) 
    {
        if (drawing) 
        {
            // Stop the timer
            timer->stop();
        }
        drawing= FALSE;
        canvas->updateZoomed();
    }
}


void SprayCan::drawDot()
{
    int dx,dy;
    int i;
    QPainter painter1;
    QPainter painter2;
    QWMatrix m;

    canvas->markModified();
    m.scale((float) 100/(canvas->zoom()), (float) 100/(canvas->zoom()));
    painter1.begin(canvas->pixmap());

    if (activeButton == LeftButton)
        painter1.setPen(leftpen);
    else
        painter1.setPen(rightpen);

    painter1.setWorldMatrix(m);

    painter2.begin(canvas->zoomedPixmap());

    if (activeButton == LeftButton)
        painter2.setPen(leftpen);
    else
        painter2.setPen(rightpen);

    painter1.drawPoint(x, y);
    painter2.drawPoint(x, y);

    for (i= 0; i < (density/20); i++) 
    {
        dx= (rand() % (2*brushsize))-brushsize;
        dy= (rand() % (2*brushsize))-brushsize;
        painter1.drawPoint(x+dx, y+dy);
        painter2.drawPoint(x+dx, y+dy);
    }
    painter1.end();
    painter2.end();
    canvas->repaint(0);
}

QPixmap SprayCan::pixmap() const
{
    return UserIcon("spraycan");
}

#include "spraycan.moc"




