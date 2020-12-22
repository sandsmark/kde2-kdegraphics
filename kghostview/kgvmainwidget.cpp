#include "kgvmainwidget.h"

KGVMainWidget::KGVMainWidget( QWidget* parent, const char* name )
  : QWidget( parent, name ) {}

void KGVMainWidget::keyPressEvent( QKeyEvent* event )
{
    if( event->key() == Key_Space ) {
	event->accept();
	emit spacePressed();
    }
}

#include "kgvmainwidget.moc"
