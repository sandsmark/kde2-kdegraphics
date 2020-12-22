#include <kdebug.h>
#include <qpushbutton.h>
#include <klocale.h>
#include <kapp.h>
#include "palettedialog.h"

paletteDialog::paletteDialog(QPixmap *p, QWidget *parent, const char *name)
  : QDialog(parent, name, TRUE)
{
  resize(340, 280);  

  pal= new paletteWidget(p, this);
  pal->move(10, 10);
  pal->resize(320, 240);

  QPushButton *dismiss = new QPushButton( i18n("Dismiss"), this );
  dismiss->setAutoResize(TRUE);
  dismiss->move(140, 250);

  connect( dismiss, SIGNAL(clicked()), SLOT(accept()) );
  connect( pal, SIGNAL(colourSelected(int)), pal, SLOT(editColour(int)) );
}

QPixmap *paletteDialog::pixmap()
{
  return pal->pixmap();
}

#include "palettedialog.moc"
