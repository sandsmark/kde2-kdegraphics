function(create_kde2_config_header)
    include(CheckIncludeFiles)
    check_include_files(fcntl.h HAVE_FCNTL_H)
    check_include_files(sys/time.h HAVE_SYS_TIME_H)
    check_include_files(unistd.h HAVE_UNISTD_H)
    check_include_files(stdlib.h HAVE_STDLIB_H)
    check_include_files(paths.h HAVE_PATHS_H)

    include(CheckFunctionExists)
    check_function_exists("usleep" HAVE_USLEEP)
    check_function_exists("setenv" HAVE_SETENV)
    check_function_exists("unsetenv" HAVE_UNSETENV)

    configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)
    include_directories(${PROJECT_BINARY_DIR})
    add_definitions(-DHAVE_CONFIG_H)
endfunction()

