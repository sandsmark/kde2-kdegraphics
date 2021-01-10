#include "pluginmenu.h"
#include "imagewindow.h"
#include "ifapp.h"
#include "miscbindings.h"
#include <qdir.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kdesktopfile.h>
#include <ksimpleconfig.h>
#include <qimage.h>
#include <qpixmap.h>
#include <kmessagebox.h>

extern KIFImageWindow *pluginHackWinPtr; // naughty, naughty ;-)

KIFPluginMenu::KIFPluginMenu(KIFImageWindow *window, QWidget *parent,
                             const char *name)
    : QPopupMenu(parent, name)
{
    effect_ptr = NULL;
    handle = 0;
    win = window;
    pluginList.setAutoDelete(true);
    connect(this, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));
    reload();
}

void KIFPluginMenu::reload()
{
    clear();
    pluginList.clear();
    insertItem(i18n("Reload plugins"));
    QStringList dirList = KGlobal::dirs()->findDirs("appdata", "plugins");
    // make a sorted list of effects and their plugins
    for(QStringList::Iterator it = dirList.begin(); it != dirList.end(); ++it){
        QDir dir(*it, "*.desktop");
        const QFileInfoList *list = dir.entryInfoList();
        QFileInfoListIterator fiIterator(*list);
        QFileInfo *fi;
        while((fi = fiIterator.current())){
            if(KDesktopFile::isDesktopFile(fi->absFilePath())){
                KSimpleConfig config(fi->absFilePath(), true);
                config.setDesktopGroup();
                QString libStr = config.readEntry("X-KDE-Library");
                QStringList effectList = config.readListEntry("Effects");
                QStringList::Iterator eIterator = effectList.begin();
                while(eIterator != effectList.end()){
                    PluginItem *pItem = new PluginItem(*eIterator, libStr);
                    pluginList.inSort(pItem);
                    ++eIterator;
                }
            }
            ++fiIterator;
        }
    }
    QListIterator<PluginItem> it(pluginList);
    while(it.current()){
        insertItem((*it)->effect());
        ++it;
    }
}

void KIFPluginMenu::slotActivated(int id)
{
    static bool dlregistered = false;
    int index = indexOf(id);
    if(index == 0){
        if(handle){
            qWarning("Unloading previous plugin");
            lt_dlclose(handle);
            handle = 0;
            effect_ptr = NULL;
        }
        reload();
        return;
    }
    PluginItem *pItem = pluginList.at(index-1);
    QString libStr = pItem->plugin();
    QString effectStr = pItem->effect();
    if(effect_ptr && currentLoadedStr == libStr){
        qWarning("Plugin already loaded");
        // initalize
        effect_ptr(effectStr, (QWidget*) parent(), win->pixmap(),
                   win->image(), win->selectRect(), showProgress, incProgress,
                   updateImage);
        // process
        proc_ptr();
    }
    else{
        QString libPath = libStr;
        libPath += ".la";
        libPath = KGlobal::dirs()->findResource("lib", libPath);
        if(!libPath){
            KMessageBox::error((QWidget *)parent(),
                               i18n("Cannot find plugin for this effect!\nIt is configured as ")
                               + libStr, i18n("Pixie Plugin Error"));
            return;
        }
        if(!dlregistered){
            dlregistered = true;
            lt_dlinit();
        }
        if(handle){
            qWarning("Unloading previous plugin");
            lt_dlclose(handle);
            handle = 0;
            effect_ptr = NULL;
        }
        handle = lt_dlopen(libPath.latin1());
        if(!handle){
            KMessageBox::error((QWidget *)parent(),
                               i18n("Cannot dynamic load the plugin for this effect!\nThe plugin name is ")
                               + libStr, i18n("Pixie Plugin Error"));
            return;
        }
        currentLoadedStr = libStr;
        lt_ptr effect_func = lt_dlsym(handle, "kifinternal_init");
        effect_ptr = (void * (*)(QString &, QWidget *, QPixmap *,
                                 QImage *, QRect *, void (*)(),
                                 void (*)(), void (*)())) effect_func;
        lt_ptr proc_func = lt_dlsym(handle, "process");
        proc_ptr = (void *(*)()) proc_func;
        if(effect_ptr && proc_ptr){
            qWarning("Plugin effect init sucessfully resolved");
            // initalize
            effect_ptr(effectStr, (QWidget*) parent(), win->pixmap(),
                       win->image(), win->selectRect(),
                       showProgress, incProgress, updateImage);
            // process
            proc_ptr();
        }
        else{
            if(!effect_ptr)
                KMessageBox::error((QWidget *)parent(),
                                   i18n("Plugin was not linked with libkifplugin!\nThe plugin name is ")
                                   + libStr, i18n("Pixie Plugin Error"));
            else
                KMessageBox::error((QWidget *)parent(),
                                   i18n("Plugin lacks the required process() function!\nThe plugin name is ")
                                   + libStr, i18n("Pixie Plugin Error"));
        }
    }
    kifapp()->stopBusyCursor();

}


#include "pluginmenu.moc"
