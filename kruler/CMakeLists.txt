qt2_wrap_cpp(kruler_MOC
    SOURCES
        klineal.h
    )

kde2_kinit_executable(kruler
    SOURCES
        klineal.cpp main.cpp
        ${kruler_MOC}
    LIBS
        kde2::kfile
    )

kde2_stdicon()

install(FILES eventsrc move.wav DESTINATION ${KDE2_DATADIR}/kruler)
install(FILES kruler.desktop DESTINATION ${KDE2_APPSDIR}/Graphics)
