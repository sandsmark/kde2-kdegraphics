/*
  X11 Utility Methods for ImageMagick.
*/
#ifndef _XWINDOWS_H
#define _XWINDOWS_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#undef False
#undef True
#define XLIB_ILLEGAL_ACCESS  1
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/Xlocale.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/XWDFile.h>
#if defined(HasShape)
#include <X11/extensions/shape.h>
#endif
#if defined(HasSharedMemory)
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif
#if defined(HasDPS)
# include <DPS/dpsXclient.h>
# include <DPS/dpsXpreview.h>
#endif

/*
  Define declarations.
*/
#define MaxNumberPens  11
#define MaxNumberFonts  11
#define MaxIconSize  96

/*
  Enumeration declarations.
*/
typedef enum
{
  ForegroundStencil,
  BackgroundStencil,
  OpaqueStencil,
  TransparentStencil
} AnnotationStencil;

typedef enum
{
  UndefinedColormap,
  PrivateColormap,
  SharedColormap
} XColormapType;

/*
  Typedef declarations.
*/
typedef struct _DiversityPacket
{
  Quantum
    red,
    green,
    blue;

  unsigned short
    index;

  unsigned long
    count;
} DiversityPacket;

typedef struct _XAnnotateInfo
{
  int
    x,
    y;

  unsigned int
    width,
    height;

  AnnotationStencil
    stencil;

  double
    degrees;

  XFontStruct
    *font_info;

  char
    *text,
    geometry[MaxTextExtent];

  struct _XAnnotateInfo
    *previous,
    *next;
} XAnnotateInfo;

typedef struct _XDrawInfo
{
  int
    x,
    y;

  unsigned int
    width,
    height;

  char
    geometry[MaxTextExtent];

  double
    degrees;

  AnnotationStencil
    stencil;

  PrimitiveType
    primitive;

  Pixmap
    stipple;

  unsigned int
    line_width;

  XSegment
    line_info;

  RectangleInfo
    rectangle_info;

  unsigned int
    number_coordinates;

  XPoint
    *coordinate_info;
} XDrawInfo;

typedef struct _XImportInfo
{
  unsigned int
    frame,
    borders,
    screen,
    descend,
    silent;
} XImportInfo;

typedef struct _XPixelInfo
{
  unsigned int
    colors;

  unsigned long
    *pixels;

  XColor
    foreground_color,
    background_color,
    border_color,
    matte_color,
    highlight_color,
    shadow_color,
    depth_color,
    trough_color,
    box_color,
    pen_color,
    pen_colors[MaxNumberPens],
    *gamma_map;

  unsigned short
    box_index,
    pen_index;

  GC
    annotate_context,
    highlight_context,
    widget_context;
} XPixelInfo;

typedef struct _XResourceInfo
{
  XrmDatabase
    resource_database;

  ImageInfo
    *image_info;

  QuantizeInfo
    *quantize_info;

  unsigned int
    colors;

  unsigned int
    close_server,
    backdrop;

  char
    *background_color,
    *border_color;

  unsigned int
    border_width;

  char
    *client_name;

  XColormapType
    colormap;

  unsigned int
    color_recovery;

  unsigned int
    confirm_exit,
    debug,
    delay;

  char
    *display_gamma;

  unsigned int
    display_warnings;

  char
    *font,
    *font_name[MaxNumberFonts],
    *foreground_color;

  unsigned int
    gamma_correct;

  int
    gravity;

  char
    home_directory[MaxTextExtent],
    *icon_geometry;

  unsigned int
    iconic,
    immutable;

  char
    *image_geometry;

  unsigned int
    magnify;

  char
    *map_type,
    *matte_color,
    *name;

  unsigned int
    pause;

  char
    *pen_colors[MaxNumberPens];

  int
    quantum;

  char
    *text_font,
    *title;

  unsigned int
    update,
    undo_cache,
    use_pixmap,
    use_shared_memory;

  char
    *visual_type,
    *window_group,
    *window_id,
    *write_filename;

  Image
    *copy_image;
} XResourceInfo;

typedef struct _XWindowInfo
{
  Window
    id;

  int
    screen;

  Window
    root;

  Visual
    *visual;

  int
    class,
    depth;

  XVisualInfo
    *visual_info;

  XStandardColormap
    *map_info;

  XPixelInfo
    *pixel_info;

  XFontStruct
    *font_info;

  GC
    annotate_context,
    highlight_context,
    widget_context;

  Cursor
    cursor,
    busy_cursor;

  char
    *name,
    *geometry,
    *icon_name,
    *icon_geometry,
    *crop_geometry;

  unsigned int
    data;

  unsigned long
    flags;

  int
    x,
    y;

  unsigned int
    width,
    height,
    min_width,
    min_height,
    width_inc,
    height_inc,
    border_width,
    use_pixmap,
    immutable,
    shape,
    shared_memory;

  XImage
    *ximage,
    *matte_image;

  Pixmap
    highlight_stipple,
    shadow_stipple,
    pixmap,
    *pixmaps,
    matte_pixmap,
    *matte_pixmaps;

  int
    mask;

  XSetWindowAttributes
    attributes;

  XWindowChanges
    window_changes;

#if defined(HasSharedMemory)
  XShmSegmentInfo
    segment_info[2];
#endif

  unsigned int
    orphan,
    mapped,
    stasis;
} XWindowInfo;

typedef struct _XWindows
{
  Display
    *display;

  XStandardColormap
    *map_info,
    *icon_map;

  XVisualInfo
    *visual_info,
    *icon_visual;

  XPixelInfo
    *pixel_info,
    *icon_pixel;

  XFontStruct
    *font_info;

  XResourceInfo
    *icon_resources;

  XClassHint
    *class_hints;

  XWMHints
    *manager_hints;

  XWindowInfo
    context,
    group_leader,
    backdrop,
    icon,
    image,
    info,
    magnify,
    pan,
    command,
    widget,
    popup;

  Atom
    wm_protocols,
    wm_delete_window,
    wm_take_focus,
    im_protocols,
    im_remote_command,
    im_update_widget,
    im_update_colormap,
    im_former_image,
    im_retain_colors,
    im_next_image,
    im_exit,
    dnd_protocols;
} XWindows;


extern Export void  XProgressMonitor(const char *,const off_t,const off_t);
extern Export void  XSignalHandler(int);
extern Export void  XWarning(const WarningType,const char *,const char *);
extern Export int XError(Display *,XErrorEvent *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
