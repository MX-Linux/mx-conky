# **********************************************************************
# * Copyright (C) 2017-2025 MX Authors
# *
# * Authors: Adrian
# *          MX Linux <http://mxlinux.org>
# *
# * This file is part of mx-conky.
# *
# * mx-conky is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * mx-conky is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with mx-conky.  If not, see <http://www.gnu.org/licenses/>.
# **********************************************************************/

cmake_minimum_required(VERSION 3.16)
project(mx-conky VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to use clang for testing builds
option(USE_CLANG "Use clang compiler" OFF)
if(USE_CLANG)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_CXX_COMPILER_ID "Clang")
    message(STATUS "Using clang compiler")
endif()

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets LinguistTools)

# Qt6 configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type -Werror=switch")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=uninitialized")

# Add compiler-specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR USE_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-stack-address")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-local-addr")
    # Suppress false positive LTO warnings with Qt6 QHash
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-alloc-size-larger-than")
endif()

# Handle different build types
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DNDEBUG)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=auto")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto=auto")
    endif()
endif()

# Handle 32-bit builds and debian builds differently
if(CMAKE_SIZEOF_VOID_P EQUAL 4 OR DEFINED ENV{DEB_BUILD_OPTIONS})
    # Reduce strict flags for 32-bit compatibility or debian builds
    message(STATUS "32-bit build or debian build detected, reducing compiler strictness")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

add_definitions(-DQT_DEPRECATED_WARNINGS)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/cmd.cpp
    src/conkyitem.cpp
    src/conkymanager.cpp
    src/conkylistwidget.cpp
    src/settingsdialog.cpp
    src/conkycustomizedialog.cpp
    src/previewdialog.cpp
)

# Header files
set(HEADERS
    src/mainwindow.h
    src/cmd.h
    src/conkyitem.h
    src/conkymanager.h
    src/conkylistwidget.h
    src/settingsdialog.h
    src/conkycustomizedialog.h
    src/previewdialog.h
)

# Translation files - dynamically find all .ts files
file(GLOB TS_FILES "translations/*.ts")

# Resource files
set(RESOURCES
    images.qrc
)

# Create the executable
add_executable(mx-conky ${SOURCES} ${HEADERS} ${RESOURCES})

# Add include directories
target_include_directories(mx-conky PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link Qt6 libraries
target_link_libraries(mx-conky Qt6::Core Qt6::Widgets)

# Handle translations - generate .qm files in translations/ directory
set(QM_FILES)
foreach(TS_FILE ${TS_FILES})
    get_filename_component(TS_BASENAME ${TS_FILE} NAME_WE)
    set(QM_FILE ${CMAKE_CURRENT_SOURCE_DIR}/translations/${TS_BASENAME}.qm)
    add_custom_command(
        OUTPUT ${QM_FILE}
        COMMAND Qt6::lrelease ${TS_FILE} -qm ${QM_FILE}
        DEPENDS ${TS_FILE}
        COMMENT "Generating ${QM_FILE}"
    )
    list(APPEND QM_FILES ${QM_FILE})
endforeach()

add_custom_target(translations ALL DEPENDS ${QM_FILES})
add_dependencies(mx-conky translations)

# Add cleanup target to remove .qm files before lupdate
add_custom_target(clean-translations
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_SOURCE_DIR}/translations/*.qm
    COMMENT "Cleaning translation files"
)

# Add lupdate target for updating translation files
add_custom_target(lupdate
    COMMAND Qt6::lupdate ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h -ts ${TS_FILES}
    DEPENDS clean-translations
    COMMENT "Updating translation files with lupdate"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Installation
install(TARGETS mx-conky DESTINATION bin)
