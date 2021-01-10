
INCLUDES = $(all_includes) 

lib_LTLIBRARIES = libpixie_color.la

libpixie_color_la_SOURCES = channeledit.cpp dualimagergb.cpp imagehsv.cpp \
	singlecolors.cpp widgets.cpp init.cpp
METASOURCES = AUTO


#EXTRA_DIST = $(lnk_DATA)

libpixie_color_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -module -no-undefined
libpixie_color_la_LIBADD = $(LIB_KDEUI)

###KMAKE-start (don't edit or delete this block)

###KMAKE-end
