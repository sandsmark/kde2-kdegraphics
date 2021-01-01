kde2_stdicon()

qt2_wrap_cpp(kcoloredit_MOC
    SOURCES
        colorselector.h
        gradientselection.h
        imageselection.h
        kcoloreditdoc.h
        kcoloredit.h
        kcoloreditview.h
        kxycolorselector.h
        kzcolorselector.h
        loadpalettedlg.h
        paletteview.h
        paletteviewscrolledarea.h
        texteditselection.h
    )

kde2_kinit_executable(kcoloredit
    SOURCES
        kzcolorselector.cpp imageselection.cpp texteditselection.cpp
        gradientselection.cpp colorselector.cpp kxycolorselector.cpp
        paletteview.cpp paletteviewscrolledarea.cpp editablestreamhistory.cpp
        color.cpp palette.cpp loadpalettedlg.cpp kcoloreditview.cpp
        kcoloreditdoc.cpp kcoloredit.cpp
        main.cpp
        ${kcoloredit_MOC}
    LIBS
        kde2::kfile
    )
kde2_kinit_executable(kcolorchooser
    SOURCES
        kcolorchooser.cpp
        ${kcoloredit_MOC}
    LIBS
        kde2::kdeui
    )

kde2_stdicon()

install(FILES kcoloredit.desktop kcolorchooser.desktop DESTINATION ${KDE2_APPSDIR}/Graphics)
