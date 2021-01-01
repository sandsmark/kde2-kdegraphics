qt2_wrap_ui(kview_UI
    SOURCES
        kviewconfdialog_base.ui
        imagelistdialog_base.ui
        knumdialog_base.ui
        infowin_base.ui
    )

qt2_wrap_cpp(kview_MOC
    SOURCES
        filter.h
        imagelistdialog.h
        infowin.h
        kfilteraction.h
        kviewconfdialog.h
        kview.h
        viewer.h
    )

qt2_wrap_cpp(libkview_MOC
    SOURCES
        canvas.h
        kview_asyncio.h
    )

qt2_wrap_cpp(kviewpart_MOC
    SOURCES
        kview_view.h
    )

kde2_library(libkview
    SOURCES
        canvas.cpp
        kview_asyncio.cc
        ${libkview_MOC}
    LIBS
        kde2::kdeui
        kde2::kfile
        kde2::kdeprint
    )

kde2_module(libkviewpart
    SOURCES
        kview_view.cc
        ${kviewpart_MOC}
    LIBS
        kde2::kview
    )

kde2_kinit_executable(kview
    SOURCES
        main.cpp viewer.cpp kview.cpp colour.cpp filter.cpp filtlist.cpp
        kcproc.cpp kviewconfdialog.cpp
        imagelistdialog.cpp
        knumdialog.cpp kfilteraction.cpp infowin.cpp
        ${kview_UI}
        ${kview_MOC}
    LIBS
        kde2::kview
    )

kde2_stdicon()

install(FILES kview_part.rc kview_popup.rc kviewui.rc DESTINATION ${KDE2_DATADIR}/kview)
install(FILES kview.desktop DESTINATION ${KDE2_APPSDIR}/Graphics)
install(FILES pics/rotate.png DESTINATION ${KDE2_DATADIR}/kview/toolbar)

