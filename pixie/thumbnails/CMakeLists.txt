qt2_wrap_cpp(libpixie_thumb_MOC
    SOURCES
        hotlistbox.h
        thumbiconview.h
        thumbtoplevel.h
        thumbtree.h
    )

kde2_module(libpixie_thumb
    SOURCES
        thumbiconview.cpp thumbpropsdlg.cpp thumbtree.cpp
        thumbpreview.cpp thumbtoplevel.cpp  hotlistbox.cpp
        ${libpixie_thumb_MOC}
    LIBS
        kde2::ksycoca
    )


