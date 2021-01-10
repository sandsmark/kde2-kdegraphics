qt2_wrap_cpp(libpixie_color_MOC
    SOURCES
        channeledit.h
        dualimagergb.h
        imagehsv.h
        singlecolors.h
        widgets.h
    )

kde2_module(libpixie_color
    SOURCES
        channeledit.cpp dualimagergb.cpp imagehsv.cpp
        singlecolors.cpp widgets.cpp init.cpp
        ${libpixie_color_MOC}
    LIBS
        kde2::kdeui

    )

