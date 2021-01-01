qt2_wrap_cpp(libkmultipage_MOC
    SOURCES
        kmultipage.h
        centeringScrollview.h
    )

kde2_library(libkmultipage
    SOURCES
        kmultipage.cpp centeringScrollview.cpp
        ${libkmultipage_MOC}
    LIBS
        kde2::kparts
    )

qt2_wrap_cpp(kviewshellpart_MOC
    SOURCES
        marklist.h
        kviewpart.h
        scrollbox.h
        gotodialog.h
    )

kde2_library(libkviewerpart
    SOURCES
        kviewpart.cpp marklist.cpp scrollbox.cpp gotodialog.cpp
        ${kviewshellpart_MOC}
    LIBS
        kde2::kparts
        kde2::kmultipage
    )

qt2_wrap_cpp(kviewshell_MOC
    SOURCES
        kviewshell.h
    )
kde2_kinit_executable(kviewshell
    SOURCES
        main.cpp kviewshell.cpp
        ${kviewshell_MOC}
    LIBS
        kde2::kviewerpart
    )

install(FILES kviewshell.rc DESTINATION ${KDE2_DATADIR}/kviewshell)
install(FILES kviewerpart.rc DESTINATION ${KDE2_DATADIR}/kviewerpart)
install(FILES paper-formats DESTINATION ${KDE2_DATADIR}/kviewshell)

# TODO kpagetest
#qt2_wrap_cpp(kpagetest_MOC
#    SOURCES
#        kpagetest.h
#)
#
