#include <qwmatrix.h>
#include <qpainter.h>
#include <kdebug.h>
#include "navwidget.h"

NavWidget::NavWidget(QWidget *parent, const char *name) 
  : QWidget(parent, name), view(0)
{

}

NavWidget::~NavWidget() 
{

}

void NavWidget::paintEvent(QPaintEvent *)
{
    if ( !view )
      return;

    double xfactor, yfactor;
    if ( view->width() > view->height() ) { 
        xfactor= ((double) width() / view->width());
        yfactor= ((double) width() / view->width());
    }
    else { 
        xfactor= ((double) height() / view->height());
        yfactor= ((double) height() / view->height());
    }

    QWMatrix matrix;
    matrix.scale(xfactor, yfactor);

    QPainter p;
    p.begin(this);
    p.setWorldMatrix(matrix);
    p.drawPixmap(0, 0, *view);
    p.end();
}

void NavWidget::sizeChanged()
{
    repaint(FALSE);
}

void NavWidget::setPixmap(QPixmap *p)
{
    view = p;
    repaint(FALSE);
}

void NavWidget::pixmapChanged()
{
  // kdDebug(4400) << "NavWidget::pixmapChanged()\n" << endl;
  repaint(FALSE);
}

void NavWidget::viewportChanged(int, int, int, int)
{
  // Use this to draw the view rectangle
}

#include "navwidget.moc"
