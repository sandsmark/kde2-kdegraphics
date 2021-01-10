#include "moviewindow.h"
#include <qmovie.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <kmessagebox.h>
#include <klocale.h>

MovieWindow::MovieWindow(const QString &fileName, QWidget *parent,
                         const char *name)
    : QWidget(parent, name)
{
    movie = new QMovie(fileName);
    if(movie->isNull()){
        KMessageBox::error(this, i18n("File is not an animated GIF or PNG."),
                           i18n("AniForge Error!"));
        delete this;
        return;
    }
    movie->connectUpdate(this, SLOT(slotMovieUpdate()));
    setCaption(i18n("AniPixie"));
    show();
}

QSize MovieWindow::sizeHint() const
{
    qWarning("In sizeHint()");
    qWarning("sizeHint: %d, %d", movie->framePixmap().size().width(),
            movie->framePixmap().size().height());
    return(movie->framePixmap().size());
}

void MovieWindow::closeEvent(QCloseEvent *)
{
    qWarning("In ani close event");
    delete movie;
    delete this;
}

void MovieWindow::paintEvent(QPaintEvent *ev)
{
    QRect r = ev->rect();
    bitBlt(this, r.topLeft(), &movie->framePixmap(), r, Qt::CopyROP);
}

void MovieWindow::slotMovieUpdate()
{
    repaint(false);
}
    
void MovieWindow::slotPlay()
{
    movie->restart();
}

void MovieWindow::slotPause()
{
    movie->pause();
}

void MovieWindow::slotStop()
{
    movie->pause();
}

void MovieWindow::slotRestart()
{
    movie->restart();
}

#include "moviewindow.moc"




