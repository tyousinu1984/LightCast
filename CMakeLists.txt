cmake_minimum_required(VERSION 3.20)
project(lightcast_native LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "LightCast native layer targets Windows 10/11 only (Windows.Graphics.Capture, D3D11, Media Foundation, WASAPI).")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# native/src/{capture,camera,audio,graphics,encoder,filters} get added here
# with add_subdirectory() as each one gains real source files.
