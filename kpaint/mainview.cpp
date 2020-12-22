#include <qlayout.h>
#include <qscrollview.h>

#include "canvas.h"

#include "mainview.h"
#include "mainview.moc"

MainView::MainView(QWidget *parent, const char *name)
  : QWidget(parent,name)
{
    lm = new QBoxLayout(this, QBoxLayout::TopToBottom);

    v = new QScrollView( this );
    lm->addWidget(v);

    int w = 300;
    int h = 200;
    c = new Canvas(w, h, v->viewport());
    connect( c, SIGNAL( sizeChanged() ), SLOT( updateLayout() ) );

    int x, y;
    if ( width() > w )
       x = width()/2 - w/2;
    else
       x = 0;
    if ( height() > h )
       y = height()/2 - h/2;
    else
       y = 0;
    v->addChild( c, x, y );

}

MainView::~MainView()
{
}

void MainView::updateLayout()
{
    int w = c->width();
    int h = c->height();
    int x, y;
    if ( width() > w )
       x = width()/2 - w/2;
    else
       x = 0;
    if ( height() > h )
       y = height()/2 - h/2;
    else
       y = 0;
    v->moveChild( c, x, y );
}

void MainView::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    updateLayout();
}

