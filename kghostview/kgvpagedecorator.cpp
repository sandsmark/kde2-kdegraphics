#include <qdrawutil.h>
#include <qregion.h>

#include "kgvpagedecorator.h"
    
KGVPageDecorator::KGVPageDecorator( QWidget* parent, const char* name )
  : QHBox( parent, name )
  , _margin( 5 ), _borderWidth( 1 ), _shadowOffset( 2, 2 )
{
    setFrameStyle( Box | Plain );
    setLineWidth( _margin + _borderWidth );
    setBackgroundMode( NoBackground );
    setAutoMask( true );
}

bool KGVPageDecorator::eventFilter( QObject* o, QEvent* e )
{
    switch( e->type() ) {
    case QEvent::MouseButtonPress: 
    case QEvent::MouseButtonRelease: 
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
	return event( e );
    default:
	;
    }
    return QHBox::eventFilter( o, e );
}

void KGVPageDecorator::childEvent( QChildEvent* e )
{
    if( e->child()->isWidgetType() && e->inserted() )
	e->child()->installEventFilter( this );
}

void KGVPageDecorator::drawFrame( QPainter* p )
{
    QRect r( frameRect().topLeft()     + QPoint(_margin,_margin), 
	     frameRect().bottomRight() - QPoint(_margin,_margin) );
	     
    if( !r.isValid() ) 
	return;
    
    const QColorGroup& cg = colorGroup();

    r.moveCenter( r.center() + _shadowOffset );
    qDrawPlainRect( p, r, cg.shadow(), _shadowOffset.manhattanLength() );
    
    r.moveCenter( r.center() - _shadowOffset );
    qDrawPlainRect( p, r, cg.foreground(), _borderWidth );
}

void KGVPageDecorator::drawFrameMask( QPainter* p )
{
    QRect r( frameRect().topLeft()     + QPoint(_margin,_margin), 
	     frameRect().bottomRight() - QPoint(_margin,_margin) );

    if( !r.isValid() )
	return;

    QColorGroup cg( color1, color1, color1, color1, color1, color1, color1,
		    color1, color0 );
    
    r.moveCenter( r.center() + _shadowOffset );
    qDrawPlainRect( p, r, cg.foreground(), _shadowOffset.manhattanLength() );
    
    r.moveCenter( r.center() - _shadowOffset );
    qDrawPlainRect( p, r, cg.foreground(), _borderWidth );
}
