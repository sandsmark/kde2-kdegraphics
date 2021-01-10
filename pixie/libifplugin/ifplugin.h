#ifndef __KIFPLUGIN_H
#define __KIFPLUGIN_H

class QImage;
class QRect;
class QString;
class QWidget;
class QPixmap;

/**
 * Programming interface for KDE Pixie plugins. You can use this in
 * plugins to get access to image data, the selection rectangle, etc...
 */
class KIFPlugin{
public:
    /**
     * Returns the current image. This will always be non-null and 32bpp.
     */
    static QImage* image();
    /**
     * Returns the selection rectangle. This will be invalid
     * (isValid() == false) if there is no selection made.
     */
    static QRect* selection();
    /**
     * Returns the effect name requested from the .desktop file's "Effects"
     * string list.
     */
    static QString effectName();
    /**
     * Returns a widget suitable to use as a parent for dialogs.
     */
    static QWidget *parent();
    /**
     * By default the image pixmap *is not* updated from any changes you made
     * to the image data. This is to allow you to easily cancel before making
     * any changes. If you change the image data call updateImage to tell
     * Pixie to update. You should call this at least on exit if you
     * make changes.
     */
    static void updateImage();
    /**
     * Pixie supports a little rotating busy cursor. This starts it.
     */
    static void startAniCursor();
    /**
     * Increments the rotation of the busy cursor. You can call this during
     * long operations to show something is going on while avoiding using a
     * full progress dialog.
     */
    static void incAniCursor();

    static QImage copySelection();
    static void putSelection(QImage &img);
};

// internal function to initalize plugins, you don't need to bother with this
extern "C" void kifinternal_init(QString &, QWidget *, QPixmap *, QImage *,
                                 QRect *, void *(sa)(), void *(ia)(),
                                 void *(ui)());

#endif

