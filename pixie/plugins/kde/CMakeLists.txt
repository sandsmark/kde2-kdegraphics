qt2_wrap_cpp(libkif_kdeeffects_MOC SOURCES dialogs.h)
kde2_module(libkif_kdeeffects
    SOURCES
        dialogs.cpp effects.cpp
        ${libkif_kdeeffects_MOC}
    LIBS
        kde2::kifplugin
        kde2::kdeui
    )
install(FILES kdeeffects.desktop DESTINATION ${KDE2_DATADIR}/pixie/plugins)

