kde2_library(libminimagick
    SOURCES
        animate.c colors.c effects.c magick.c segment.c utility.c annotate.c
        compress.c enhance.c memory.c shear.c zoom.c blob.c decorate.c error.c
        monitor.c signature.c cache.c delegates.c gems.c montage.c timer.c
        cache_io.c draw.c image.c quantize.c transform.c
    HEADERS
        mini-magick/animate.h
        mini-magick/cache.h
        mini-magick/compress.h
        mini-magick/error.h
        mini-magick/magick.h
        mini-magick/monitor.h
        mini-magick/utility.h
        mini-magick/api.h
        mini-magick/cache_io.h
        mini-magick/defines.h
        mini-magick/gems.h
        mini-magick/memory.h
        mini-magick/quantize.h
        mini-magick/version.h
        mini-magick/blob.h
        mini-magick/classify.h
        mini-magick/delegates.h
        mini-magick/image.h
        mini-magick/timer.h
    LIBS
        m
    )

target_include_directories(libminimagick PRIVATE mini-magick)


