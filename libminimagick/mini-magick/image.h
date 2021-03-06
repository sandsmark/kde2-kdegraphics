/*
  ImageMagick Image Methods.
*/
#ifndef _IMAGE_H
#define _IMAGE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined(QuantumLeap)
/*
  Color quantum is [0..65535].
*/
#define DownScale(quantum)  (((unsigned long) (quantum)) >> 8)
#define HexColorFormat "#%04x%04x%04x"
#define MaxRGB  65535L
#define QuantumDepth  16
#define UpScale(quantum)  (257*((unsigned long) (quantum)))
#define XDownScale(color)  ((unsigned long) (color))
#define XUpScale(color)  ((unsigned long) (color))

typedef unsigned short Quantum;
#else
/*
  Color quantum is [0..255].
*/
#define DownScale(quantum)  ((unsigned long) (quantum))
#define HexColorFormat "#%02x%02x%02x"
#define MaxRGB  255L
#define QuantumDepth  8
#define UpScale(quantum)  ((unsigned long) (quantum))
#define XDownScale(color)  (((unsigned long) (color)) >> 8)
#define XUpScale(color)  (257*((unsigned long) (color)))

typedef unsigned char Quantum;
#endif
#define Opaque  MaxRGB
#define Transparent  0

/*
  3D effects.
*/
#define AccentuateModulate  UpScale(80)
#define HighlightModulate  UpScale(125)
#define ShadowModulate  UpScale(135)
#define DepthModulate  UpScale(185)
#define TroughModulate  UpScale(110)

/*
  Typedef declarations.
*/
typedef struct _BlobInfo
{
  unsigned int
    mapped;

  char
    *data;

  off_t
    offset;

  size_t
    length,
    extent,
    quantum;
} BlobInfo;

typedef void* Cache;

typedef struct _ColorlistInfo
{
  char
    *name;

  unsigned char
    red,
    green,
    blue;
} ColorlistInfo;

typedef struct _FrameInfo
{
  int
    x,
    y;

  unsigned int
    width,
    height;

  int
    inner_bevel,
    outer_bevel;
} FrameInfo;

typedef unsigned short IndexPacket;

typedef struct _PixelPacket
{
#if defined(WORDS_BIGENDIAN)
  Quantum
    red,
    green,
    blue,
    opacity;
#else
#if defined(WIN32)
  Quantum
    blue,
    green,
    red,
    opacity;
#else
  Quantum
    opacity,
    red,
    green,
    blue;
#endif
#endif
} PixelPacket;

typedef struct _ImageInfo
{
  /*
    Blob member.
  */
  BlobInfo
    blob_info;

  /*
    File and image dimension members.
  */
  FILE
    *file;

  char
    filename[MaxTextExtent],
    magick[MaxTextExtent],
    unique[MaxTextExtent],
    zero[MaxTextExtent];

  unsigned int
    temporary,
    adjoin,
    subimage,
    subrange,
    depth,
    ping;

  char
    *size,
    *tile,
    *page;

  InterlaceType
    interlace;

  ResolutionType
    units;

  /*
    Compression members.
  */
  CompressionType
    compression;

  unsigned int
    quality;

  /*
    Annotation members.
  */
  char
    *server_name,
    *box,
    *font,
    *pen,
    *texture,
    *density;

  double
    pointsize;

  unsigned int
    linewidth,
    antialias;

  int
    fuzz;

  PixelPacket
    background_color,
    border_color,
    matte_color;

  /*
    Color reduction members.
  */
  unsigned int
    dither,
    monochrome;

  ColorspaceType
    colorspace;

  /*
    Animation members.
  */
  char
    *dispose,
    *delay,
    *iterations;

  unsigned int
    decode_all_MNG_objects,
    coalesce_frames,
    insert_backdrops;

  /*
    Miscellaneous members.
  */
  unsigned int
    verbose;

  PreviewType
    preview_type;

  char
    *view;

  long
    group;
} ImageInfo;

typedef struct _MontageInfo
{
  char
    filename[MaxTextExtent],
    *geometry,
    *tile,
    *title,
    *frame,
    *texture,
    *pen,
    *font;

  PixelPacket
    background_color,
    border_color,
    matte_color;

  double
    pointsize;

  unsigned int
    border_width,
    gravity,
    shadow;

  CompositeOperator
    compose;
} MontageInfo;

typedef struct _PointInfo
{
  double
    x,
    y,
    z;
} PointInfo;

typedef struct _ProfileInfo
{
  unsigned int
    length;

  unsigned char
    *info;
} ProfileInfo;

typedef struct _RectangleInfo
{
  unsigned int
    width,
    height;

  int
    x,
    y;
} RectangleInfo;

typedef struct _SegmentInfo
{
  double
    x1,
    y1,
    x2,
    y2;
} SegmentInfo;

typedef struct _Timer
{
  double
    start,
    stop,
    total;
} Timer;

typedef struct _TimerInfo
{
  Timer
    user,
    elapsed;

  TimerState
    state;
} TimerInfo;

typedef struct _ChromaticityInfo
{
  PointInfo
    red_primary,
    green_primary,
    blue_primary,
    white_point;
} ChromaticityInfo;

typedef struct _Image
{
  int
    exempt,
    status,
    temporary;

  char
    filename[MaxTextExtent];

  FILE
    *file;

  size_t
    filesize;

  int
    pipe;

  char
    magick[MaxTextExtent],
    *comments,
    *label;

  ClassType
#if defined(__cplusplus) || defined(c_plusplus)
    c_class;
#else
    class;
#endif

  unsigned int
    matte;

  CompressionType
    compression;

  unsigned int
    columns,
    rows,
    depth;

  int
    offset;

  RectangleInfo
    tile_info;

  InterlaceType
    interlace;

  unsigned int
    scene;

  char
    *montage,
    *directory;

  PixelPacket
    *colormap;

  unsigned int
    colors;

  ColorspaceType
    colorspace;

  RenderingIntent
    rendering_intent;

  double
    gamma;

  ChromaticityInfo
    chromaticity;

  ProfileInfo
    color_profile,
    iptc_profile;

  ResolutionType
    units;

  double
    x_resolution,
    y_resolution;

  char
    *signature;

  PixelPacket
    *pixels;

  unsigned short
    *indexes;

  PixelPacket
    background_color,
    border_color,
    matte_color;

  char
    *geometry;

  RectangleInfo
    page_info;

  unsigned int
    dispose,
    delay,
    iterations;

  int
    fuzz;

  FilterType
    filter;

  double
    blur;

  unsigned long
    total_colors;

  unsigned int
    mean_error_per_pixel;

  double
    normalized_mean_error,
    normalized_maximum_error;

  char
    magick_filename[MaxTextExtent];

  unsigned int
    magick_columns,
    magick_rows;

  int
    restart_animation_here,
    tainted;

  TimerInfo
    timer_info;

  unsigned int
    orphan;

  BlobInfo
    blob_info;

  Cache
    cache;

  RectangleInfo
    cache_info;

  struct _Image
    *previous,
    *list,
    *next;
} Image;

typedef struct _AnnotateInfo
{
  ImageInfo
    *image_info;

  unsigned int
    gravity;

  char
    *geometry,
    *text,
    *primitive,
    *font_name;

  double
    degrees;

  Image
    *tile;

  RectangleInfo
    bounds;
} AnnotateInfo;

typedef struct _MagickInfo
{
  char
    *tag;

  Image
    *(*decoder)(const ImageInfo *);

  unsigned int
    (*encoder)(const ImageInfo *,Image *),
    (*magick)(const unsigned char *,const unsigned int),
    adjoin,
    blob_support,
    raw;

  char
    *description;

  void
    *data;

  struct _MagickInfo
    *previous,
    *next;
} MagickInfo;

/*
  Image const declarations.
*/
extern const char
  *Alphabet,
  *BackgroundColor,
  *BorderColor,
  *DefaultPointSize,
  *DefaultTileFrame,
  *DefaultTileGeometry,
  *DefaultTileLabel,
  *ForegroundColor,
  *MatteColor,
  *LoadImageText,
  *LoadImagesText,
  *PSDensityGeometry,
  *PSPageGeometry,
  *ReadBinaryType,
  *ReadBinaryUnbufferedType,
  *SaveImageText,
  *SaveImagesText,
  *WriteBinaryType;

extern const ColorlistInfo
  XPMColorlist[235],
  XColorlist[757];

/*
  Image utilities methods.
*/
extern Export AnnotateInfo
  *CloneAnnotateInfo(const ImageInfo *,const AnnotateInfo *);

extern Export Image
  *AddNoiseImage(Image *,const NoiseType),
  *AllocateImage(const ImageInfo *),
  *AppendImages(Image *,const unsigned int),
  *AverageImages(Image *),
  *BlurImage(Image *,const double),
  *BorderImage(Image *,const RectangleInfo *),
  *ChopImage(Image *,const RectangleInfo *),
  *CloneImage(Image *,const unsigned int,const unsigned int,const unsigned int),
  *CoalesceImages(Image *),
  *ColorizeImage(Image *,const char *,const char *),
  *CreateImage(const unsigned int,const unsigned int,const char *,
    const StorageType,const void *),
  *CropImage(Image *,const RectangleInfo *),
  *DeconstructImages(Image *),
  *DespeckleImage(Image *),
  *EdgeImage(Image *,const double),
  *EmbossImage(Image *),
  *EnhanceImage(Image *),
  *FlipImage(Image *),
  *FlopImage(Image *),
  *FrameImage(Image *,const FrameInfo *),
  *GetNextImage(Image *),
  *ImplodeImage(Image *,const double),
  **ListToGroupImage(const Image *,unsigned int *),
  *MagnifyImage(Image *),
  *MedianFilterImage(Image *,const unsigned int),
  *MinifyImage(Image *),
  *MontageImages(const Image *,const MontageInfo *),
  *MorphImages(Image *,const unsigned int),
  *MosaicImages(Image *),
  *OilPaintImage(Image *,const unsigned int),
  *PingImage(const ImageInfo *),
  *ReadImage(ImageInfo *),
  *ReadImages(ImageInfo *),
  *ReduceNoiseImage(Image *),
  *RollImage(Image *,const int,const int),
  *RotateImage(Image *,const double),
  *SampleImage(Image *,const unsigned int,const unsigned int),
  *ScaleImage(Image *,const unsigned int,const unsigned int),
  *ShadeImage(Image *,const unsigned int,double,double),
  *SharpenImage(Image *,const double),
  *ShearImage(Image *,const double,const double),
  *SpreadImage(Image *,const unsigned int),
  *SteganoImage(Image *,Image *),
  *StereoImage(Image *,Image *),
  *SwirlImage(Image *,double),
  *WaveImage(Image *,const double,const double),
  *ZoomImage(Image *,const unsigned int,const unsigned int);

extern Export ImageInfo
  *CloneImageInfo(const ImageInfo *);

extern Export ImageType
  GetImageType(Image *);

extern Export int
  ParseGeometry(const char *,int *,int *,unsigned int *,unsigned int *),
  ParseImageGeometry(const char *,int *,int *,unsigned int *,unsigned int *);

extern Export MagickInfo
  *GetMagickInfo(const char *),
  *RegisterMagickInfo(MagickInfo *),
  *SetMagickInfo(const char *);

extern Export unsigned int
  AnimateImages(const ImageInfo *image_info,Image *image),
  DisplayImages(const ImageInfo *image_info,Image *image),
  GetNumberScenes(const Image *),
  IsGeometry(const char *),
  IsGrayImage(Image *),
  IsMatteImage(Image *),
  IsMonochromeImage(Image *),
  IsPseudoClass(Image *),
  IsSubimage(const char *,const unsigned int),
  IsTainted(const Image *),
  PlasmaImage(Image *,const SegmentInfo *,int,int),
  QueryColorDatabase(const char *,PixelPacket *),
  UnregisterMagickInfo(const char *);

extern Export void
  AllocateNextImage(const ImageInfo *,Image *),
  AnnotateImage(Image *,const AnnotateInfo *),
  ColorFloodfillImage(Image *,const PixelPacket *,Image *,const int x,
    const int y,const PaintMethod),
  CommentImage(Image *,const char *),
  CompositeImage(Image *,const CompositeOperator,Image *,const int,const int),
  CompressColormap(Image *),
  ContrastImage(Image *,const unsigned int),
  CycleColormapImage(Image *,const int),
  DescribeImage(Image *,FILE *,const unsigned int),
  DestroyAnnotateInfo(AnnotateInfo *),
  DestroyImage(Image *),
  DestroyImageInfo(ImageInfo *),
  DestroyImages(Image *),
  DestroyMagickInfo(),
  DestroyMontageInfo(MontageInfo *),
  DrawImage(Image *,const AnnotateInfo *),
  EqualizeImage(Image *),
  GammaImage(Image *,const char *),
  GetAnnotateInfo(const ImageInfo *,AnnotateInfo *),
  GetImageInfo(ImageInfo *),
  GetMontageInfo(MontageInfo *),
  GetPageInfo(RectangleInfo *),
  GetPixels(Image *,const int,const int,const unsigned int,const unsigned int,
    const char *,const StorageType,void *),
  GetPixelPacket(PixelPacket *),
  LabelImage(Image *,const char *),
  LayerImage(Image *,const LayerType),
  ListMagickInfo(FILE *),
  MatteFloodfillImage(Image *,const PixelPacket *,const unsigned int,
    const int x,const int y,const PaintMethod),
  MatteImage(Image *,Quantum),
  ModulateImage(Image *,const char *),
  MogrifyImage(const ImageInfo *,const int,char **,Image **),
  MogrifyImages(const ImageInfo *,const int,char **,Image **),
  NegateImage(Image *,const unsigned int),
  NormalizeImage(Image *),
  OpaqueImage(Image *,const char *,const char *),
  RaiseImage(Image *,const RectangleInfo *,const int),
  RGBTransformImage(Image *,const ColorspaceType),
  SetImage(Image *),
  SetImageInfo(ImageInfo *,const unsigned int),
  SignatureImage(Image *),
  SolarizeImage(Image *,const double),
  SortColormapByIntensity(Image *),
  SyncImage(Image *),
  TextureImage(Image *,Image *),
  ThresholdImage(Image *,const double),
  TransformHSL(const Quantum,const Quantum,const Quantum,double *,double *,
    double *),
  TransformImage(Image **,const char *,const char *),
  TransformRGBImage(Image *,const ColorspaceType),
  TransparentImage(Image *,const char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
