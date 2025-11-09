cmake_minimum_required(VERSION 3.16)
project(NotepadPlusPlusLinux 
    VERSION 1.0.0
    DESCRIPTION "Native Linux port of Notepad++ text editor"
    HOMEPAGE_URL "https://github.com/yourusername/notepad-plus-plus-linux"
    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Build type default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find Qt6 (or Qt5 as fallback)
find_package(Qt6 COMPONENTS Core Widgets QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 COMPONENTS Core Widgets REQUIRED)
    set(QSCINTILLA_LIB qscintilla2_qt5)
    message(STATUS "Using Qt5")
else()
    set(QSCINTILLA_LIB qscintilla2_qt6)
    message(STATUS "Using Qt6")
endif()

# Find QScintilla
find_library(QSCINTILLA_LIBRARY ${QSCINTILLA_LIB})
if(NOT QSCINTILLA_LIBRARY)
    message(FATAL_ERROR "QScintilla library not found. Install libqscintilla2-qt6-dev (or qt5 version)")
else()
    message(STATUS "Found QScintilla: ${QSCINTILLA_LIBRARY}")
endif()

# Source files
set(SOURCES
    main.cpp
    core/Application.cpp
    core/MainWindow.cpp
    core/MainWindow_impl.cpp
    core/EditorWidget.cpp
    platform/LinuxPlatform.cpp
    utils/ConfigManager.cpp
    ui/FindReplaceDialog.cpp
    ui/FindInFilesDialog.cpp
    ui/DocumentMapWidget.cpp
    ui/DocumentMapPanel.cpp
    ui/PreferencesDialog.cpp
)

# Header files (for MOC)
set(HEADERS
    core/Application.h
    core/MainWindow.h
    core/EditorWidget.h
    platform/PlatformInterface.h
    utils/ConfigManager.h
    ui/FindReplaceDialog.h
    ui/FindInFilesDialog.h
    ui/DocumentMapWidget.h
    ui/DocumentMapPanel.h
    ui/PreferencesDialog.h
)

# Create executable
add_executable(notepadplusplus ${SOURCES} ${HEADERS})

# Link Qt libraries
if(Qt6_FOUND)
    target_link_libraries(notepadplusplus Qt6::Core Qt6::Widgets ${QSCINTILLA_LIBRARY})
else()
    target_link_libraries(notepadplusplus Qt5::Core Qt5::Widgets ${QSCINTILLA_LIBRARY})
endif()

# Compiler flags
target_compile_options(notepadplusplus PRIVATE
    -Wall -Wextra -Wpedantic
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g>
)

# Set target properties
set_target_properties(notepadplusplus PROPERTIES
    OUTPUT_NAME "notepadplusplus"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Installation
install(TARGETS notepadplusplus
    RUNTIME DESTINATION bin
    COMPONENT Runtime
)

# Install documentation
install(FILES 
    README.md
    LICENSE
    CHANGELOG.md
    CONTRIBUTING.md
    DESTINATION share/doc/notepadplusplus
    COMPONENT Documentation
)

install(DIRECTORY docs/
    DESTINATION share/doc/notepadplusplus
    COMPONENT Documentation
)

# CPack configuration
include(CPack)
set(CPACK_PACKAGE_NAME "notepadplusplus")
set(CPACK_PACKAGE_VENDOR "Notepad++ Linux Project")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Native Linux port of Notepad++ text editor")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_CONTACT "maintainer@example.com")

# DEB package configuration
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6widgets6, libqscintilla2-qt6-15")
set(CPACK_DEBIAN_PACKAGE_SECTION "editors")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

# RPM package configuration
set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase, qscintilla-qt6")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Editors")

# Display build information
message(STATUS "")
message(STATUS "Build Configuration:")
message(STATUS "  Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "")