kde2_module(libkif_magick
    SOURCES
        dialog.cpp magick.cpp
    LIBS
        kde2::kifplugin
        kde2::kdeui
        kde2::minimagick
    )
install(FILES magick.desktop DESTINATION ${KDE2_DATADIR}/pixie/plugins)

