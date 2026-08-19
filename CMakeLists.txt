cmake_minimum_required(VERSION 3.19) # for CMake's own FindX11 X11::Xtst imported target
project(xembed-sni-proxy)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# previously enabled implicitly by including KDECMakeSettings; needed for
# Q_OBJECT classes to get moc'd, and for generated headers (the D-Bus
# adaptor/interface, this one written into the build dir) to be found by
# plain #include "foo.h"
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)
include(FeatureSummary)

find_package(Qt6 REQUIRED COMPONENTS Core Gui DBus)

# previously pulled in transitively via KF6::WindowSystem; find it explicitly
# now that this project has no KDE Frameworks dependency
find_package(X11 REQUIRED COMPONENTS Xtst)
set_package_properties(X11 PROPERTIES TYPE REQUIRED)

# the XCB libraries don't ship CMake config files, and the previous
# find_package(XCB COMPONENTS ...) relied on the FindXCB.cmake module that
# came from extra-cmake-modules; go through plain pkg-config instead so this
# doesn't need any KDE-provided cmake modules either
find_package(PkgConfig REQUIRED)
pkg_check_modules(XCB REQUIRED IMPORTED_TARGET
    xcb
    xcb-xfixes
    xcb-damage
    xcb-composite
    xcb-randr
    xcb-shm
    xcb-util
    xcb-image
    xcb-icccm
    xcb-shape
)

set(XEMBED_SNI_PROXY_SOURCES
    main.cpp
    debug.cpp debug.h
    fdoselectionmanager.cpp fdoselectionmanager.h
    selectionowner.cpp selectionowner.h
    snidbus.cpp snidbus.h
    sniproxy.cpp
    xtestsender.cpp xtestsender.h
 )

qt_add_dbus_adaptor(XEMBED_SNI_PROXY_SOURCES org.kde.StatusNotifierItem.xml
                     sniproxy.h SNIProxy)

set(statusnotifierwatcher_xml org.kde.StatusNotifierWatcher.xml)
qt_add_dbus_interface(XEMBED_SNI_PROXY_SOURCES ${statusnotifierwatcher_xml} statusnotifierwatcher_interface)

add_executable(xembedsniproxy ${XEMBED_SNI_PROXY_SOURCES})

target_link_libraries(xembedsniproxy
    Qt::Core
    Qt::DBus
    Qt::Gui
    PkgConfig::XCB
    X11::Xtst
)

install(TARGETS xembedsniproxy DESTINATION ${CMAKE_INSTALL_BINDIR})
