kde2_library(libminimagick
    SOURCES
        animate.c colors.c effects.c magick.c segment.c utility.c annotate.c
        compress.c enhance.c memory.c shear.c zoom.c blob.c decorate.c error.c
        monitor.c signature.c cache.c delegates.c gems.c montage.c timer.c
        cache_io.c draw.c image.c quantize.c transform.c
    HEADERS
        mini-magic/animate.h
        mini-magic/cache.h
        mini-magic/compress.h
        mini-magic/error.h
        mini-magic/magick.h
        mini-magic/monitor.h
        mini-magic/utility.h
        mini-magic/api.h
        mini-magic/cache_io.h
        mini-magic/defines.h
        mini-magic/gems.h
        mini-magic/memory.h
        mini-magic/quantize.h
        mini-magic/version.h
        mini-magic/blob.h
        mini-magic/classify.h
        mini-magic/delegates.h
        mini-magic/image.h
        mini-magic/timer.h
    LIBS
        m
    )

target_include_directories(libminimagick PRIVATE mini-magick)


