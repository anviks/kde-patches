cmake_minimum_required(VERSION 3.16)
project(breeze-nowheel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(breezenowheel MODULE nowheelstyle.cpp)
target_link_libraries(breezenowheel PRIVATE Qt6::Widgets)

# Where Qt looks for style plugins. On Fedora this is /usr/lib64/qt6/plugins/styles.
# Override with: cmake -DSTYLE_PLUGIN_DIR=/some/path ..
if(NOT STYLE_PLUGIN_DIR)
    get_target_property(_qtcore_location Qt6::Core LOCATION)
    get_filename_component(_qtlibdir "${_qtcore_location}" DIRECTORY)
    set(STYLE_PLUGIN_DIR "${_qtlibdir}/qt6/plugins/styles")
endif()
message(STATUS "Style plugin will be installed to: ${STYLE_PLUGIN_DIR}")

install(TARGETS breezenowheel LIBRARY DESTINATION "${STYLE_PLUGIN_DIR}")
