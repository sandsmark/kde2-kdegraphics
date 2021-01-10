#include "thumbtoplevel.h"
#include "thumbtree.h"
#include "thumbpreview.h"
#include "thumbiconview.h"
#include "thumbpropsdlg.h"
#include "hotlistbox.h"
#include "kapp.h"
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kprogress.h>
#include <klineedit.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <ktoolbar.h>
#include <kmimetype.h>
#include <krun.h>
#include <qpopupmenu.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qdir.h>
#include <qfile.h>
#include <qsplitter.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qinputdialog.h>
#include <qtabwidget.h>
#include <qlabel.h>
#include <X11/Xlib.h>

void *(*setPixieURL)(const QString &urlStr);

extern "C"{
    void init(void *((setURLPtr)(const QString &urlStr)))
    {
        setPixieURL = setURLPtr;
        new KIFThumbTopLevel;
    }
}

KIFThumbTopLevel::KIFThumbTopLevel(const char *name)
    : KMainWindow(0,name)
{
    KConfig *config = KGlobal::config();
    config->setGroup("Thumbnails");
    pathStr = QDir::current().absPath();
    int sortVal = config->readNumEntry("Sorting", 0);
    if(sortVal == 6){
        sorting = QDir::Size;
        dupFirst = true;
    }
    else{
        sorting = (QDir::SortSpec)sortVal;
        dupFirst = false;
    }
    iconSize = (KIFThumbView::IconSize)config->
        readNumEntry("Size", (int)KIFThumbView::Large);
                                                            
    rightBtnMnu = new KPopupMenu(this);
    connect(rightBtnMnu, SIGNAL(activated(int)), this,
            SLOT(slotRightButtonMenu(int)));
    rightClickItem = NULL;

    rightBtnDirMnu = new KPopupMenu(this);
    rightBtnDirMnu->insertTitle(i18n("Directory Options"), DM_Title);
    rightBtnDirMnu->insertItem(i18n("Add to HotList Group..."), DM_HotList);
    connect(rightBtnDirMnu, SIGNAL(activated(int)), this,
            SLOT(slotRightButtonDirMenu(int)));
    
    KStatusBar *sBar = statusBar(); // Statusbar
    //sBar->setInsertOrder(KStatusBar::RightToLeft);
    progress = new KProgress(KProgress::Horizontal, sBar);
    sBar->addWidget(progress);
    sBar->insertItem(i18n("KDE Pixie thumbnail viewer"), 20);

    KToolBar *tBar = new KToolBar(this); // Main toolbar
    tBar->setFullSize();

    QPushButton *optBtn = new QPushButton(i18n("Options"), tBar);
    optMenu = new QPopupMenu(optBtn);
    optBtn->setPopup(optMenu);
    tBar->insertWidget(1, optBtn->sizeHint().width(), optBtn);

    sortMenu = new QPopupMenu(optMenu);
    sortMenu->insertItem(i18n("Ascending by name"), 0);
    sortMenu->insertItem(i18n("Descending by name"), 1);
    sortMenu->insertItem(i18n("Ascending by date"), 2);
    sortMenu->insertItem(i18n("Descending by date"), 3);
    sortMenu->insertItem(i18n("Ascending by size"), 4);
    sortMenu->insertItem(i18n("Descending by size"), 5);
    sortMenu->insertItem(i18n("Ascending size/same sizes first"), 6);
    sortMenu->setItemChecked(sorting, true);
    connect(sortMenu, SIGNAL(activated(int)), this, SLOT(slotSortMenu(int)));

    optMenu->insertItem(i18n("Sorting"), sortMenu, 1);

    sizeMenu = new QPopupMenu(optMenu);
    sizeMenu->insertItem(i18n("Small"), (int)KIFThumbView::Small);
    sizeMenu->insertItem(i18n("Medium"), (int)KIFThumbView::Medium);
    sizeMenu->insertItem(i18n("Large"), (int)KIFThumbView::Large);
    sizeMenu->setItemChecked(iconSize, true);
    connect(sizeMenu, SIGNAL(activated(int)), this, SLOT(slotSizeMenu(int)));

    optMenu->insertItem(i18n("Thumbnail Size"), sizeMenu, 2);

    optMenu->insertItem(i18n("Images and Folders only"), this,
                        SLOT(slotImageOnly()), 0, 3);
    optMenu->setItemChecked(3, config->readBoolEntry("ImageOnly", false));
    
    tBar->insertButton("up", 2, SIGNAL(clicked()), this,
                       SLOT(slotUpDir()), true, i18n("Move up a directory"));
    tBar->insertButton("gohome", 3, SIGNAL(clicked()), this,
                       SLOT(slotHomeClicked()), true,
                       i18n("Go to home"));
    tBar->insertButton("reload", 4, SIGNAL(clicked()), this,
                       SLOT(slotUpdateView()), true,
                       i18n("Reload"));
    tBar->insertButton("thumb", 5, SIGNAL(clicked()), this,
                       SLOT(slotUpdateClicked()), true,
                       i18n("Update thumbnails"));
    tBar->insertButton("kwin", 6, SIGNAL(clicked()), this,
                       SLOT(slotNewWindow()), true,
                       i18n("New window"));
    tBar->insertButton("stop", 7, SIGNAL(clicked()), this,
                       SLOT(slotStopClicked()), false,
                       i18n("Stop"));

    addToolBar(tBar);
    tBar = new KToolBar(this); // second toolbar
    tBar->setFullSize();

    /* causing a segfault...
    tBar->insertLined(QDir::current().absPath(), 1, SIGNAL(returnPressed()),
                      this, SLOT(slotPathEdit()), true, i18n("Directory Path"));
    pathEdit = tBar->getLined(1);
    tBar->setItemAutoSized(1);
    */
    pathEdit = new QLineEdit(QDir::current().absPath(), tBar);
    connect(pathEdit, SIGNAL(returnPressed()), this, SLOT(slotPathEdit()));
    
    tBar->insertWidget(1, pathEdit->sizeHint().width(), pathEdit);
    tBar->setItemAutoSized(1);

    addToolBar(tBar);

    QSplitter *mainSplitter = new QSplitter(Horizontal, this);
    QSplitter *vSplitter = new QSplitter(Vertical, mainSplitter);

    QTabWidget *treeTabWidget = new QTabWidget(vSplitter);
    tree = new KIFDirTree(treeTabWidget);
    tree->setPath(pathStr);
    connect(tree, SIGNAL(doubleClicked(QListViewItem *)), this,
            SLOT(slotTreeClicked(QListViewItem *)));
    treeTabWidget->addTab(tree, SmallIcon("folder.png"), i18n("Folders"));

    hotListBox = new KIFHotListBox(treeTabWidget);
    connect(hotListBox, SIGNAL(doubleClicked(QListBoxItem *)), this,
            SLOT(slotHotListBoxClicked(QListBoxItem *)));
    connect(hotListBox, SIGNAL(dropOnList(int, QStringList &, QDropEvent::Action)), this,
            SLOT(slotHotListBoxDrop(int, QStringList &, QDropEvent::Action)));
    connect(hotListBox, SIGNAL(addCWD()), this, SLOT(slotAddHotListCWD()));
    connect(hotListBox, SIGNAL(deleteHotItem(unsigned int)), this,
            SLOT(slotDelHotList(unsigned int)));

    treeTabWidget->addTab(hotListBox, SmallIcon("mini-ray"), i18n("HotList"));
    
    vSplitter->setResizeMode(treeTabWidget, QSplitter::FollowSizeHint);

    QVBox *vsBox = new QVBox(vSplitter);
    previewBtn = new QPushButton(i18n("Enable Preview"), vsBox);
    connect(previewBtn, SIGNAL(clicked()), this,
            SLOT(slotPreviewStatClicked()));
    preview = new KIFThumbPreview(vsBox);
    preview->setEnabled(false);
    vSplitter->setResizeMode(vsBox, QSplitter::KeepSize);

    mainSplitter->setResizeMode(vSplitter, QSplitter::FollowSizeHint);
    
    mainWidget = new KIFThumbView(mainSplitter);
    mainWidget->setIconOnly(optMenu->isItemChecked(3));
    mainSplitter->setResizeMode(mainWidget, QSplitter::Stretch);
    connect(mainWidget, SIGNAL(doubleClicked(QIconViewItem *)), this,
            SLOT(slotItemDblClicked(QIconViewItem *)));
    connect(mainWidget, SIGNAL(rightButtonPressed(QIconViewItem *, const QPoint &)),
            this, SLOT(slotRightButtonPressed(QIconViewItem *, const QPoint &)));
    connect(mainWidget, SIGNAL(clicked(QIconViewItem *)), this,
            SLOT(slotItemClicked(QIconViewItem *)));
    connect(mainWidget, SIGNAL(updateMe()), this,
            SLOT(slotUpdateView()));

    connect(mainWidget, SIGNAL(enableStopButton(bool)), this,
            SLOT(slotEnableStop(bool)));
    connect(this, SIGNAL(stopClicked()), mainWidget,
            SLOT(slotStopClicked()));

    connect(mainWidget, SIGNAL(updateProgress(int)), progress,
            SLOT(setValue(int)));
    
    setCentralWidget(mainSplitter);

    
    //resize(sizeHint().width() < 450 ? 450 : sizeHint().width(),
    //       sizeHint().height());
    resize(550, 400); // hack - sizehint's are messed up

    show();
    setCaption(i18n("KDE Pixie Thumbnails"));
    reloadHotList();
    mainWidget->update(pathStr, iconSize, sorting, false, dupFirst);

    //connect(kifapp(), SIGNAL(hideAllWindows()), this, SLOT(slotHide()));
    //connect(kifapp(), SIGNAL(showAllWindows()), this, SLOT(slotShow()));
}

void KIFThumbTopLevel::closeEvent(QCloseEvent *)
{
    delete this;
}

void KIFThumbTopLevel::slotPathEdit()
{
    if(QFile::exists(pathEdit->text())){
        QFileInfo fi(pathEdit->text());
        if(!fi.isDir()){
            KMessageBox::error(this, i18n("Invalid directory entered!"));
            pathEdit->setText(pathStr);
        }
        else{
            pathStr = pathEdit->text();
            mainWidget->update(pathStr, iconSize, sorting, false, dupFirst);
            tree->setPath(pathEdit->text());
        }
    }
    else{
        KMessageBox::error(this, i18n("Directory not found!"));
        pathEdit->setText(pathStr);
    }
}

void KIFThumbTopLevel::slotTreeClicked(QListViewItem *i)
{
    if(i){
        KIFDirItem *item = (KIFDirItem *)i;
        mainWidget->update(item->fullPath(), iconSize, sorting, false, dupFirst);
        pathEdit->setText(item->fullPath());
        pathStr = item->fullPath();
    }
}

void KIFThumbTopLevel::slotItemDblClicked(QIconViewItem *i)
{
    if(i){
        QString iStr = i->text();
        
        if(((KIFThumbViewItem*)i)->parentDir())
            iStr = "../";

        QFileInfo fi(pathStr + "/" + iStr);
        if(fi.isDir()){
            QDir d(pathStr + "/" + iStr);
            pathStr = d.absPath(); // get rid of crap in the URL
            pathEdit->setText(pathStr);
            tree->setPath(pathStr);
            mainWidget->update(pathStr, iconSize, sorting, false, dupFirst);
        }
        else{
            // if it's an image, display and add to filelist - else exec
            // default app
            KURL url;
            url.setPath(pathStr + "/" + iStr);
            if(KMimeType::findByURL(url, true, true)->name().left(6) ==
               "image/"){
                // image
                emit fileSelected(pathStr + "/" + iStr);
                setPixieURL(pathStr + "/" + iStr);
                updateStatus(iStr, true);
            }
            else{
                // data
                updateStatus(i18n("Running application for ") + iStr, true);
                new KRun(url);
            }
        }
    }
}

void KIFThumbTopLevel::slotItemClicked(QIconViewItem *i)
{
    if(i){
        if(((KIFThumbViewItem*)i)->parentDir())
            return;
        QFileInfo fi(pathStr + "/" + i->text());
        if(!fi.isDir()){
            emit fileHighlighted(pathStr + "/" + i->text());
            preview->setFileName(pathStr + "/" + i->text());
            updateStatus(i->text());
        }
    }
}

void KIFThumbTopLevel::updateStatus(const QString &fileStr, bool ignorePreview)
{
    QFileInfo fi(pathStr + '/' + fileStr);
    
    QString statusStr, sizeStr;
    double size = fi.size();
    if (size > 1024)
    {
        size /= 1024;
        if (size > 1024)
        {
            statusStr = i18n("%1,%2 %3 MB, Last Modified: %4");
            size /= 1024;
        }
        else
            statusStr = i18n("%1,%2 %3 KB, Last Modified: %4");
        sizeStr = KGlobal::locale()->formatNumber( size, 1 );
    }
    else
    {
        if (size == 1)
          statusStr = i18n("%1,%2 %3 byte, Last Modified: %4");
        else
          statusStr = i18n("%1,%2 %3 bytes, Last Modified: %4");
        sizeStr = KGlobal::locale()->formatNumber( (int)size, 0 );
    }

    QString rangeStr;
    // only display size if image already loaded for preview
    if (!ignorePreview && preview->isEnabled() && !preview->isError())
      rangeStr = i18n("width x height if preview is on", " %1x%2,")
        .arg( KGlobal::locale()->formatNumber( preview->pixmap().width(), 0 ) )
        .arg( KGlobal::locale()->formatNumber( preview->pixmap().height(), 0 ) );

    statusStr = statusStr
      .arg( fileStr )
      .arg( rangeStr )
      .arg( sizeStr )
      .arg( KGlobal::locale()->formatDateTime( fi.lastModified() ) );

    statusBar()->changeItem(statusStr, 20);
}
    

void KIFThumbTopLevel::slotRightButtonPressed(QIconViewItem *i,
                                              const QPoint &pos)
{
    rightClickItem = i;
    if(i){
        if(i->dropEnabled()){
            if(((KIFThumbViewItem *)i)->parentDir())
                tmpStr = "../";
            else
                tmpStr = i->text();
            rightBtnDirMnu->exec(pos);
        }
        else
            rightBtnMnu->exec(pos);
    }
}

void KIFThumbTopLevel::slotSortMenu(int i)
{
    KConfig *config = KGlobal::config();
    config->setGroup("Thumbnails");
    int tmp;
    for(tmp=0; tmp <= 6; ++tmp)
        sortMenu->setItemChecked(tmp, false);
    sortMenu->setItemChecked(i, true);

    if(i == 6)
        dupFirst = true;
    else
        dupFirst = false;
    sorting = i;

    config->writeEntry("Sorting", i);
    config->sync();
    mainWidget->update(pathStr, iconSize, sorting, false, dupFirst);
}

void KIFThumbTopLevel::slotSizeMenu(int i)
{
    sizeMenu->setItemChecked((int)KIFThumbView::Small, false);
    sizeMenu->setItemChecked((int)KIFThumbView::Medium, false);
    sizeMenu->setItemChecked((int)KIFThumbView::Large, false);
    sizeMenu->setItemChecked(i, true);
    
    KConfig *config = KGlobal::config();
    config->setGroup("Thumbnails");
    config->writeEntry("Size", i);
    config->sync();
    iconSize = (KIFThumbView::IconSize)i;
    mainWidget->update(pathStr, iconSize, sorting, false, dupFirst);
}

void KIFThumbTopLevel::slotUpdateClicked()
{
    mainWidget->update(pathStr, iconSize, sorting, true, dupFirst);
}

void KIFThumbTopLevel::slotSetProgress(int )
{

}

void KIFThumbTopLevel::slotPreviewStatClicked()
{
    preview->setEnabled(!preview->isEnabled());
    if(preview->isEnabled()){
        previewBtn->setText(i18n("Disable Preview"));
        QIconViewItem *i = mainWidget->currentItem();
        if(i)
            preview->setFileName(pathStr + "/" + i->text());
    }
    else
        previewBtn->setText(i18n("Enable Preview"));
}

void KIFThumbTopLevel::slotHide()
{
    wasVisible = isVisible();
    hide();
}

void KIFThumbTopLevel::slotShow()
{
    if(wasVisible)
        show();
}

void KIFThumbTopLevel::slotRightButtonDirMenu(int id)
{
    if(id == DM_HotList){ // only one right now
        addHotListPath(pathStr + "/" + tmpStr);
    }
}


void KIFThumbTopLevel::slotRightButtonMenu(int id)
{
    if(!rightClickItem)
        return;

    if(id == TM_Props){
        KIFThumbProperties dlg(*rightClickItem->pixmap(),
                               pathStr + "/" + rightClickItem->text(),
                               this);
        dlg.exec();
    }
    else if(id == TM_Rename){
        QString result = QInputDialog::getText(i18n("Rename"),
                                               i18n("Rename ") +
                                               rightClickItem->text() +
                                               i18n(" to:"),
                                               rightClickItem->text(),
                                               0, this);
        if(result != QString::null){
            QDir dir(pathStr + "/");
            if(!dir.rename(rightClickItem->text(), result))
                KMessageBox::error(this, i18n("Unable to rename file."));
            else if(result.at(0) != QChar('.') && result.at(0) != QChar('/')){
                // rename any thumbnails
                if(QFile::exists(pathStr + "/.pics/large/" +
                                 rightClickItem->text()))
                    if(!dir.rename(pathStr + "/.pics/large/" +
                                   rightClickItem->text(),
                                   pathStr + "/.pics/large/" +
                                   result))
                        qWarning("Pixie: Unable to rename thumbnail");

                if(QFile::exists(pathStr + "/.pics/small/" +
                                 rightClickItem->text()))
                    if(!dir.rename(pathStr + "/.pics/small/" +
                                   rightClickItem->text(),
                                   pathStr + "/.pics/small/" +
                                   result))
                        qWarning("Pixie: Unable to rename thumbnail");

                if(QFile::exists(pathStr + "/.pics/med/" +
                                 rightClickItem->text()))
                    if(!dir.rename(pathStr + "/.pics/med/" +
                                   rightClickItem->text(),
                                   pathStr + "/.pics/med/" +
                                   result))
                        qWarning("Pixie: Unable to rename thumbnail");
                mainWidget->update(pathStr, iconSize, sorting, false, dupFirst);
            }

        }
    }
    else if(id == TM_Delete){
        QStringList delList;
        QIconViewItem *i = mainWidget->firstItem();
        while(i){
            if(i->isSelected())
                delList.append(i->text());
            i = i->nextItem();
        }
        if(!delList.count()){
            KMessageBox::error(this, i18n("No files selected."));
            return;
        }
        if(KMessageBox::questionYesNoList(this,
                                          i18n("Delete the following files?"),
                                          delList, i18n("Confirm Delete"))==0)
            return;

        for(QStringList::Iterator it = delList.begin(); it != delList.end();
            ++it){
            if(!QFile::remove(pathStr+"/"+(*it)))
                KMessageBox::error(this, i18n("Unable to delete file."));
            else{
                // delete any existing thumbnails
                if(QFile::exists(pathStr + "/.pics/large/" + (*it)))
                    if(!QFile::remove(pathStr + "/.pics/large/" + (*it)))
                        qWarning("Pixie: Unable to delete thumbnail");

                if(QFile::exists(pathStr + "/.pics/small/" + (*it)))
                    if(!QFile::remove(pathStr + "/.pics/small/" + (*it)))
                        qWarning("Pixie: Unable to delete thumbnail");

                if(QFile::exists(pathStr + "/.pics/med/" + (*it)))
                    if(!QFile::remove(pathStr + "/.pics/med/" + (*it)))
                        qWarning("Pixie: Unable to delete thumbnail");
            }
        }
        mainWidget->update(pathStr, iconSize, sorting, false, dupFirst);
    }
    else if(id != TM_Title && id != TM_Title2){
        unsigned int i;
        for(i = 0; i <  hotListBox->count(); ++i){
            if(hotListBox->text(i) == rightBtnMnu->text(id)){
                QStringList opList;
                KURL dest(hotList[i]);
                QIconViewItem *i = mainWidget->firstItem();
                while(i){
                    if(i->isSelected())
                        opList.append("file:" + pathStr + "/" + i->text());
                    i = i->nextItem();
                }
                if(!opList.count()){
                    KMessageBox::error(this, i18n("No files selected."));
                    return;
                }

                Window root;
                Window child;
                int root_x, root_y, win_x, win_y;
                uint keybstate;
                XQueryPointer(qt_xdisplay(), qt_xrootwin(), &root, &child,
                              &root_x, &root_y, &win_x, &win_y, &keybstate );
                if (keybstate & ShiftMask)
                    connect(KIO::move(opList, dest), SIGNAL(result(KIO::Job *)),
                            this, SLOT(slotJobFinished(KIO::Job *)));
                else
                    (void)KIO::copy(opList, dest);
                break;
            }
        }
    }
}

void KIFThumbTopLevel::slotUpdateView()
{
    mainWidget->update(pathStr, iconSize, sorting, false, dupFirst);
}

void KIFThumbTopLevel::slotHomeClicked()
{
    pathStr = QDir::homeDirPath();
    pathEdit->setText(pathStr);
    tree->setPath(pathStr);
    mainWidget->update(pathStr, iconSize, sorting, false, dupFirst);
}

void KIFThumbTopLevel::slotSetStatus(const QString &str)
{
    statusBar()->changeItem(str, 2);
}

void KIFThumbTopLevel::slotNewWindow()
{
    KIFThumbTopLevel *w = new KIFThumbTopLevel;
    connect(w, SIGNAL(fileSelected(const QString &)), kapp->mainWidget(),
            SLOT(slotAddAndSetURL(const QString &)));

}

void KIFThumbTopLevel::slotUpDir()
{
    QDir dir(pathStr);
    if(dir.cdUp()){
        pathStr = dir.absPath();
        pathEdit->setText(pathStr);
        tree->setPath(pathStr);
        mainWidget->update(pathStr, iconSize, sorting, false, dupFirst);
    }
}

void KIFThumbTopLevel::reloadHotList()
{
    hotList.clear();
    hotListBox->clear();
    rightBtnMnu->clear();

    rightBtnMnu->insertTitle(i18n("Thumb Options"), TM_Title);
    rightBtnMnu->insertItem(i18n("Rename..."), TM_Rename);
    rightBtnMnu->insertItem(i18n("Delete"), TM_Delete);
    rightBtnMnu->insertSeparator();
    rightBtnMnu->insertItem(i18n("Properties..."), TM_Props);
    rightBtnMnu->insertTitle(SmallIcon("mini-ray"),
                                     i18n("Send to Hotlist (Shift to move)"),
                                     TM_Title2);

    hotListBox->insertItem(BarIcon("kfm_home", KIcon::SizeMedium),
                           i18n("Home"), 0);
    rightBtnMnu->insertItem(SmallIcon("kfm_home"), i18n("Home"));
    hotList.append(QDir::homeDirPath());
    
    KConfig *config = KGlobal::config();
    config->setGroup("Hotlist");
    QStringList nameList = config->readListEntry("Names");
    QStringList pathList = config->readListEntry("Paths");

    if(!nameList.count() || ! pathList.count())
        return;
    int i=1;
    for(QStringList::Iterator it = nameList.begin(); it != nameList.end();
        ++it, ++i){
        hotListBox->insertItem(BarIcon("folder", KIcon::SizeMedium),
                               (*it), i);
        rightBtnMnu->insertItem(SmallIcon("folder"), (*it));
    }
    for(QStringList::Iterator pit = pathList.begin(); pit != pathList.end();
        ++pit)
        hotList.append((*pit));

}

void KIFThumbTopLevel::addHotListPath(const QString &path)
{
    QFileInfo fi(path);
    if(!fi.isDir()){
        qWarning("Pixie: Hotlist addition on invalid path!");
        return;
    }

    QString labelStr = QInputDialog::getText(i18n("HotList Name"),
                                             i18n("Please enter a symbolic name to use when\n\
accessing this directory from the hotlist."), fi.baseName(), 0, this);
    if(!labelStr)
        return;

    KConfig *config = KGlobal::config();
    config->setGroup("Hotlist");
    QStringList nameList = config->readListEntry("Names");
    QStringList pathList = config->readListEntry("Paths");
    nameList.append(labelStr);
    pathList.append(fi.absFilePath());
    config->writeEntry("Names", nameList);
    config->writeEntry("Paths", pathList);
    config->sync();
    reloadHotList();
}

void KIFThumbTopLevel::slotDelHotList(unsigned int id)
{
    KConfig *config = KGlobal::config();
    config->setGroup("Hotlist");
    QStringList nameList;
    QStringList pathList;
    unsigned int i;
    // skip home dir
    for(i=1; i < hotListBox->count(); ++i){
        if(i != id){
            nameList.append(hotListBox->text(i));
            pathList.append(hotList[i]);
        }
    }
    config->writeEntry("Names", nameList);
    config->writeEntry("Paths", pathList);
    config->sync();
    reloadHotList();
}

void KIFThumbTopLevel::slotAddHotListCWD()
{
    addHotListPath(pathStr);
}

void KIFThumbTopLevel::slotHotList(int id)
{
    QString newPath = hotList[id];
    pathEdit->setText(newPath);
    slotPathEdit();
}

void KIFThumbTopLevel::slotHotListBoxClicked(QListBoxItem *item)
{
    // I think arrays indexes with variables in overloaded template operators
    // is legal?
    QString newPath = hotList[hotListBox->index(item)];
    pathEdit->setText(newPath);
    slotPathEdit();
}

void KIFThumbTopLevel::slotJobFinished(KIO::Job *)
{
    slotUpdateView();
}

void KIFThumbTopLevel::slotHotListBoxDrop(int id, QStringList &urlList,
                                          QDropEvent::Action a)
{
    // no need to pay attention to job
    KURL dest("file:" + hotList[id]);
    switch(a){
    case QDropEvent::Move:
        (void)KIO::move(urlList, dest);
        break;
    case QDropEvent::Copy:
        (void)KIO::copy(urlList, dest);
        break;
    case QDropEvent::Link:
        KIO::link(urlList, dest);
        break;
    default:
        break;
    }
}

void KIFThumbTopLevel::slotEnableStop(bool on)
{
    toolBar()->setItemEnabled(7, on);
}

void KIFThumbTopLevel::slotStopClicked()
{
    emit stopClicked();
}

void KIFThumbTopLevel::slotImageOnly()
{
    KConfig *config = KGlobal::config();
    QString oldGrp = config->group();

    config->setGroup("Thumbnails");
    bool imageOnly = !optMenu->isItemChecked(3);
    config->writeEntry("ImageOnly", imageOnly);
    optMenu->setItemChecked(3, imageOnly);
    config->sync();
    config->setGroup(oldGrp);
    mainWidget->setIconOnly(imageOnly);
    mainWidget->update(pathStr, iconSize, sorting, false, dupFirst);
}


#include "thumbtoplevel.moc"

