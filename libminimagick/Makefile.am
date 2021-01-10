CFLAGS = -O2 -DNDEBUG
INCLUDES = -I$(srcdir)/mini-magick/ $(all_includes) 

SUBDIRS = mini-magick

lib_LTLIBRARIES = libminimagick.la
libminimagick_la_LDFLAGS = -version-info 5:1

libminimagick_la_SOURCES = animate.c colors.c effects.c magick.c segment.c \
	utility.c annotate.c compress.c enhance.c memory.c shear.c zoom.c \
        blob.c decorate.c error.c monitor.c signature.c cache.c delegates.c \
 	gems.c montage.c timer.c cache_io.c draw.c image.c quantize.c
	transform.c       

libminimagick_la_METASOURCES = AUTO

