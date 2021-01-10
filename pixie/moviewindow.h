#ifndef __MOVIEWINDOW_H
#define __MOVIEWINDOW_H

#include <qwidget.h>
class QMovie;

class MovieWindow : public QWidget
{
    Q_OBJECT
public:
    MovieWindow(const QString &filename, QWidget *parent=0,
                const char *name=0);
    QSize sizeHint() const;
public slots:
    void slotPlay();
    void slotPause();
    void slotStop();
    void slotRestart();
    void slotMovieUpdate();
protected:
    void paintEvent(QPaintEvent *ev);
    void closeEvent(QCloseEvent *ev);
    QMovie *movie;
};

#endif

