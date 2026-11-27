cmake_minimum_required(VERSION 3.14)
project(TimeCooker VERSION 1.0.0)

# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Discover IPLUG2_DIR if not already set (for independent builds)
if(NOT DEFINED IPLUG2_DIR)
  set(IPLUG2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." CACHE PATH "iPlug2 root directory")
endif()

# Include iPlug2 CMake configuration
include(${IPLUG2_DIR}/iPlug2.cmake)

# IGraphics backend/renderer can be set via command line:
#   cmake -DIGRAPHICS_BACKEND=SKIA -DIGRAPHICS_RENDERER=GL3 ..
# Defaults: NANOVG/METAL on macOS, NANOVG/GL2 on Windows
# See iPlug2/Scripts/cmake/IGraphics.cmake for all options

find_package(iPlug2 REQUIRED)

set(PEDAL_SOURCES "")
include(${CMAKE_CURRENT_SOURCE_DIR}/pedal.cmake)

# Build VST3 format (AUv3 is opt-in)
# To include AUv3: FORMATS ALL_AUV3
# To select specific formats: FORMATS APP VST3 AU
iplug_add_plugin(${PROJECT_NAME}
  FORMATS VST3
  SOURCES
    TimeCooker.cpp
    ${PEDAL_SOURCES}
    resources/resource.h
    ../iPlug2/WDL/resample.cpp
  RESOURCES
    resources/fonts/Roboto-Regular.ttf
)

# Force C++20 for the VST3 target
set_property(TARGET TimeCooker-vst3 PROPERTY CXX_STANDARD 20)
set_property(TARGET TimeCooker-vst3 PROPERTY CXX_STANDARD_REQUIRED ON)

target_sources(TimeCooker-vst3 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/main.rc"
)
