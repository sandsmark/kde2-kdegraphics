qt2_wrap_cpp(pixie_MOC
    SOURCES
        bigwindow.h
        dndlistbox.h
        htmlexport.h
        ifapp.h
        imagelist.h
        imagewindow.h
        moviewindow.h
        pluginmenu.h
        screengrab.h
        sizedialog.h
        slidedialog.h
    )

kde2_kinit_executable(pixie
    SOURCES
        main.cpp
        imagelist.cpp
        imagewindow.cpp
        pluginmenu.cpp
        moviewindow.cpp
        slidedialog.cpp
        imagelistitem.cpp
        sizedialog.cpp
        ifapp.cpp
        bigwindow.cpp
        htmlexport.cpp
        dndlistbox.cpp
        screengrab.cpp
        miscbindings.cpp
        ${pixie_MOC}
    LIBS
        kde2::ksycoca
        kde2::kfile
    )

kde2_stdicon()

install(FILES pixie.desktop DESTINATION ${KDE2_APPSDIR}/Graphics)

