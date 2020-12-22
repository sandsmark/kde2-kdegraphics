/*
* kview.h -- Declaration of class KView.
* Generated by newclass on Wed Oct 15 01:26:27 EST 1997.
*/
#ifndef SSK_KVIEW_H
#define SSK_KVIEW_H

#include <qlist.h>
#include <qptrdict.h>
#include <kapp.h>

class KFilterList;
class KFiltMenuFactory;
class KImageViewer;
#include "viewer.h" //gcc-2.7.2

/**
* KView application object.
* @author Sirtaj Singh Kang (taj@kde.org)
* @version $Id: kview.h 74274 2000-12-10 16:31:29Z mkretz $
*/
class KView : public QObject
{
  Q_OBJECT
public:
  /**
  * KView Constructor
  */
  KView();

  /**
  * KView Destructor
  */
  virtual ~KView();

  /**
  * Execute the application.
  */
  virtual int exec();

  const QImage *currentSelection() const;

private slots:

  void newViewer();
  void closeViewer( KImageViewer * );

  void setCutBuffer( QPixmap * );

protected:
  /**
  * Create and initialize a new viewer widget and
  * return it.
  */
  KImageViewer *makeViewer();

private:
  KApplication _app;

  void registerBuiltinFilters();

  KFilterList *_filters;

  QList<KImageViewer> *_viewers;

  QPixmap *_cutBuffer;
};

#endif // SSK_KVIEW_H
