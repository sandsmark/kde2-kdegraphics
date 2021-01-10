kde2_stdicon()

install(FILES 
        back.png brightness+.png forward.png stop.png
        brightness-.png play.png scaleup.png scaledown.png contrast+.png
        contrast-.png fliph.png flipv.png thumb.png
        mini-run.png mini-ray.png kwin.png
    DESTINATION ${KDE2_DATADIR}/pixie/toolbar)

install(FILES 
        logo.png progress1.png progress2.png inc2.png inc2high.png
        dec2.png dec2high.png thumbwallpaper.png
    DESTINATION ${KDE2_DATADIR}/pixie)
