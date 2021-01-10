#ifndef __PLUGINMENU_H
#define __PLUGINMENU_H

#include <qpopupmenu.h>
#include <qlist.h>
#include <ltdl.h>

class QImage;
class QPixmap;
class KIFImageWindow;

class PluginItem
{
public:
    PluginItem(const QString &effectStr, const QString &pluginStr)
        {eStr = effectStr; pStr = pluginStr;}
    QString& effect(){return(eStr);}
    QString& plugin(){return(pStr);}
protected:
    QString eStr, pStr;
};

class PluginList : public QList<PluginItem>
{
public:
    PluginList(){;}
    int compareItems(Item s1, Item s2)
        {return(((PluginItem *)s1)->
                effect().compare(((PluginItem *)s2)->effect()));}
};

class KIFPluginMenu : public QPopupMenu
{
    Q_OBJECT
public:
    KIFPluginMenu(KIFImageWindow *window, QWidget *parent=0, const char *name=0);
protected slots:
    void slotActivated(int);
protected:
    void pluginUpdateImage();
    void reload();

    PluginList pluginList;
    QString currentLoadedStr;
    void * (*effect_ptr)(QString &effect, QWidget *, QPixmap *,
                         QImage *, QRect *, void (*)(), void (*)(),
                         void (*)());
    void * (*proc_ptr)();
    KIFImageWindow *win;
    lt_dlhandle handle;
};

#endif


