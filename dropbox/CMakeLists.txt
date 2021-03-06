#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

subdirs(icons)

add_definitions(-DTRANSLATION_DOMAIN=\"kipiplugin_dropbox\")

set(kipiplugin_dropbox_PART_SRCS
    mpform.cpp
    plugin_dropbox.cpp
    dbwidget.cpp
    dbwindow.cpp
    dbalbum.cpp
    dbtalker.cpp
   )

add_library(kipiplugin_dropbox MODULE ${kipiplugin_dropbox_PART_SRCS})

target_link_libraries(kipiplugin_dropbox
                      PRIVATE
                      Qt5::Network

                      KF5::Kipi
                      KF5::I18n
                      KF5::WindowSystem
                      KF5kipiplugins
                     )

configure_file(kipiplugin_dropbox.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dropbox.desktop)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/kipiplugin_dropbox.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kipiplugin_dropbox                                     DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES   kipiplugin_dropboxui.rc                                DESTINATION ${KXMLGUI_INSTALL_DIR}/kipi)
