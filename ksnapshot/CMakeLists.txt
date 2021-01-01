qt2_wrap_ui(ksnapshot_UI
    SOURCES
        ksnapshotbase.ui
    )

qt2_wrap_cpp(ksnapshot_MOC
    SOURCES
        ksnapshot.h
    )

kde2_kidl(ksnapshot_IDL
    SOURCES
        ksnapshotiface.h
    )

kde2_kinit_executable(ksnapshot
    SOURCES
        ksnapshotiface_skel.cpp
        ksnapshot.cpp
        main.cpp
        ${ksnapshot_UI}
        ${ksnapshot_MOC}
        ${ksnapshot_IDL}
    LIBS
        kde2::kfile
        kde2::ksycoca
    COMPILE_DEFINITIONS
        -DKSNAPVERSION=\"0.4.1\"
    )

kde2_stdicon()

install(FILES ksnapshot.desktop DESTINATION ${KDE2_APPSDIR}/Graphics)
