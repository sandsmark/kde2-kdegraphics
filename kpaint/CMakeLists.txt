qt2_wrap_cpp(kpaint_MOC
    SOURCES
        areaselect.h
        brush.h
        canvas.h
        canvassize.h
        circle.h
        depthdialog.h
        ellipse.h
        infodialog.h
        kpaint.h
        line.h
        mainview.h
        manager.h
        navwidget.h
        palettedialog.h
        palette.h
        pen.h
        properties.h
        rectangle.h
        roundangle.h
        spraycan.h
        text.h
        tool.h
    )

kde2_kinit_executable(kpaint
    SOURCES
        main.cpp canvas.cpp kpaint.cpp manager.cpp infodialog.cpp palette.cpp
        palettedialog.cpp canvassize.cpp properties.cpp tool.cpp ellipse.cpp
        circle.cpp line.cpp brush.cpp spraycan.cpp rectangle.cpp pen.cpp
        areaselect.cpp depthdialog.cpp roundangle.cpp navwidget.cpp
        mainview.cpp text.cpp
        ${kpaint_MOC}
    LIBS
        kde2::kfile
    )

kde2_stdicon()

install(FILES kpaint.rc DESTINATION ${KDE2_DATADIR}/kpaint)
install(FILES kpaint.desktop DESTINATION ${KDE2_APPSDIR}/Graphics)


install(
    FILES
        toolpics/circle.xpm
        toolpics/areaselect.xpm
        toolpics/brush.xpm
        toolpics/line.xpm
        toolpics/pen.xpm
        toolpics/spraycan.xpm
        toolpics/ellipse.xpm
        toolpics/rectangle.xpm
        toolpics/roundangle.xpm
        toolpics/largetext.xpm
    DESTINATION
        ${KDE2_DATADIR}/kpaint/toolbar
    )
# TODO: what to do about images/, it doesn't seem to be installed?
