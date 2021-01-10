#ifndef __IMAGELIST_H
#define __IMAGELIST_H

#include <ktmainwindow.h>
#include <kaccel.h>
#include "imagewindow.h"
#include "dndlistbox.h"
class QListBoxItem;
class QTimer;

class KIFImageList : public KTMainWindow
{
    Q_OBJECT
public:
    KIFImageList(const char *name=0);
    ~KIFImageList();
    KIFImageWindow *window(){return(imageWindow);}
public slots:
    void slotToggleFileList();
    void slotThumbnails();
    // these can be either urls or local file paths
    void slotAddURL(const QString &urlStr);
    void slotAddAndSetURL(const QString &urlStr);
    void slotURLSelected(QListBoxItem *i);
    void slotSetTopItem();
protected slots:
    void slotOpen();
    void slotSave();
    void slotSaveAs();
    void slotPrint();
    void slotCopy();
    void slotPaste();
    void slotScale();
    void slotScaleIn();
    void slotScaleOut();
    void slotDouble();
    void slotShrink();
    void slotBack();
    void slotForward();
    void slotReload();
    void slotBright();
    void slotDim();
    void slotMirrorHoriz();
    void slotMirrorVert();
    void slotInvert();
    void slotIncContrast();
    void slotDecContrast();
    void slotCrop();
    void slotImageColor();
    void slotSingleColor();
    void slotGenerateHtml();
    void slotGrab();
    void slotRotate45();
    void slotPlayAni();
    void slotSlideShow();
    void slotSlideShowTimer();
    void slotDropFinished();
    void slotHide();
    void slotShow();
    void slotQuit();
    void slotSelectAll(){fileList->selectAll(true);}
    void slotDeselectAll(){fileList->clearSelection();}
protected:
    void keyPressEvent(QKeyEvent *ev);
    void closeEvent(QCloseEvent *ev);
    int findURLIndex(const QString &url);
    void loadColorPlugin();

    void *(*singleColorDlg)();
    void *(*imageColorDlg)();

    KAccel *keys;
    
    KIFDndListBox *fileList;
    KIFImageWindow *imageWindow;
    bool wasVisible, dlregistered;
    QTimer *showTimer;
    QPopupMenu *fileMnu; // needed to change SlideShow entry
    int slideShowID;
};

#endif
