cmake_minimum_required(VERSION 3.21)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "Use a build directory instead: cmake -B build or run generate.bat.")
endif()

set(CMAKE_INTERMEDIATE_DIR_STRATEGY "SHORT" CACHE STRING "")

project(CAFFeinated LANGUAGES C CXX)

if(NOT MSVC)
    message(FATAL_ERROR "This project targets MSVC only at the moment.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(TP "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

foreach(sub glfw nativefiledialog zlib squish imgui)
    if(NOT EXISTS "${TP}/${sub}/.git")
        message(FATAL_ERROR
            "Submodule thirdparty/${sub} is empty. git submodule update --init --recursive --force might help.")
    endif()
endforeach()

add_library(glfw3 STATIC
    "${TP}/glfw/src/context.c"
    "${TP}/glfw/src/init.c"
    "${TP}/glfw/src/input.c"
    "${TP}/glfw/src/monitor.c"
    "${TP}/glfw/src/platform.c"
    "${TP}/glfw/src/vulkan.c"
    "${TP}/glfw/src/window.c"
    "${TP}/glfw/src/egl_context.c"
    "${TP}/glfw/src/osmesa_context.c"
    "${TP}/glfw/src/null_init.c"
    "${TP}/glfw/src/null_monitor.c"
    "${TP}/glfw/src/null_window.c"
    "${TP}/glfw/src/null_joystick.c"
    "${TP}/glfw/src/win32_module.c"
    "${TP}/glfw/src/win32_time.c"
    "${TP}/glfw/src/win32_thread.c"
    "${TP}/glfw/src/win32_init.c"
    "${TP}/glfw/src/win32_joystick.c"
    "${TP}/glfw/src/win32_monitor.c"
    "${TP}/glfw/src/win32_window.c"
    "${TP}/glfw/src/wgl_context.c")

target_include_directories(glfw3 PUBLIC "${TP}/glfw/include")
target_compile_definitions(glfw3 PRIVATE _GLFW_WIN32 UNICODE _UNICODE _CRT_SECURE_NO_WARNINGS)

add_library(nfd STATIC "${TP}/nativefiledialog/src/nfd_win.cpp")
target_include_directories(nfd PUBLIC "${TP}/nativefiledialog/src/include")
target_compile_definitions(nfd PRIVATE UNICODE _UNICODE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(nfd PUBLIC ole32 uuid shell32)

file(GLOB ZLIB_SOURCES CONFIGURE_DEPENDS "${TP}/zlib/*.c")
add_library(zlibstatic STATIC ${ZLIB_SOURCES})
target_include_directories(zlibstatic PUBLIC "${TP}/zlib")
target_compile_definitions(zlibstatic PRIVATE
    _CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_DEPRECATE)

file(GLOB SQUISH_SOURCES CONFIGURE_DEPENDS "${TP}/squish/*.cpp")
add_library(squish STATIC ${SQUISH_SOURCES})
target_include_directories(squish PUBLIC "${TP}/squish")
target_compile_definitions(squish PRIVATE _CRT_SECURE_NO_WARNINGS)
set_target_properties(squish PROPERTIES DEBUG_OUTPUT_NAME squishd)

file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS "${TP}/imgui/*.cpp")
list(APPEND IMGUI_SOURCES
    "${TP}/imgui/backends/imgui_impl_glfw.cpp"
    "${TP}/imgui/backends/imgui_impl_opengl3.cpp")

file(GLOB IMGUI_HEADERS CONFIGURE_DEPENDS "${TP}/imgui/*.h")
list(APPEND IMGUI_HEADERS
    "${TP}/imgui/backends/imgui_impl_glfw.h"
    "${TP}/imgui/backends/imgui_impl_opengl3.h"
    "${TP}/imgui/backends/imgui_impl_opengl3_loader.h")

file(GLOB STB_HEADERS CONFIGURE_DEPENDS "${TP}/stb/*.h")

set(CODE "${CMAKE_CURRENT_SOURCE_DIR}/code")
set(RES "${CMAKE_CURRENT_SOURCE_DIR}/resource")

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${CODE}/*.cpp")
file(GLOB_RECURSE APP_HEADERS CONFIGURE_DEPENDS "${CODE}/*.h")
list(APPEND APP_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/resource.h")

file(GLOB RES_IMAGES CONFIGURE_DEPENDS "${RES}/*.png")
file(GLOB RES_FONTS CONFIGURE_DEPENDS "${RES}/*.ttf")
set(RES_ASSETS ${RES_IMAGES} ${RES_FONTS} "${RES}/exeIcon.ico")
set_source_files_properties(${RES_ASSETS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(CAFFeinated
    ${APP_SOURCES}
    ${APP_HEADERS}
    ${IMGUI_SOURCES}
    ${IMGUI_HEADERS}
    ${STB_HEADERS}
    ${RES_ASSETS}
    "${TP}/glad/src/glad.c"
    MumboGarage.rc)

target_include_directories(CAFFeinated PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${TP}/imgui" "${TP}/imgui/backends"
    "${TP}/glad/include" "${TP}/stb")

target_compile_definitions(CAFFeinated PRIVATE
    _CRT_SECURE_NO_WARNINGS UNICODE _UNICODE)

target_link_libraries(CAFFeinated PRIVATE glfw3 nfd zlibstatic squish)

target_link_options(CAFFeinated PRIVATE /FORCE:MULTIPLE /IGNORE:4006)

set_target_properties(CAFFeinated PROPERTIES
    WIN32_EXECUTABLE $<CONFIG:Release>
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
             PROPERTY VS_STARTUP_PROJECT CAFFeinated)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(glfw3 nfd zlibstatic squish PROPERTIES FOLDER "thirdparty")

source_group("Source Files\\Imported" FILES
    "${TP}/glad/src/glad.c" ${IMGUI_SOURCES})
source_group("Source Files\\Ghoulies" FILES
    "${CODE}/GhoulBundle.cpp"
    "${CODE}/GhoulDemand.cpp"
    "${CODE}/GhoulFileTypes.cpp")
source_group("Source Files\\Rare Replay" FILES "${CODE}/RPK.cpp")
source_group("Source Files\\Xenon" FILES
    "${CODE}/XUIString.cpp"
    "${CODE}/XWaveBank.cpp")
source_group("Source Files\\Pinata" FILES "${CODE}/PinataDBBundle.cpp")
source_group("Source Files\\Kameo" FILES "${CODE}/KameoDatabase.cpp")
source_group("Source Files\\Conker" FILES "${CODE}/ConkerFileTypes.cpp")
source_group("Source Files\\PDZ" FILES "${CODE}/DarkPackage.cpp")
source_group("Source Files\\Program Segments" FILES
    "${CODE}/LoadingProcess.cpp"
    "${CODE}/LoctextWindow.cpp"
    "${CODE}/MarkerWindow.cpp"
    "${CODE}/MessageHandle.cpp"
    "${CODE}/ScriptWindow.cpp"
    "${CODE}/VehicleWindow.cpp"
    "${CODE}/Program Segments/BundleSeg.cpp")
source_group("Header Files\\Imported" FILES ${IMGUI_HEADERS} ${STB_HEADERS})
source_group("Header Files\\Base" FILES "${CODE}/AssetId.h")
source_group("Header Files\\Base\\Scripts" FILES "${CODE}/BaseScript.h")
source_group("Header Files\\Base\\Markers" FILES "${CODE}/BaseMarker.h")
source_group("Header Files\\Base\\ML" FILES "${CODE}/MLTypes.h")
source_group("Header Files\\Ghoulies" FILES
    "${CODE}/GhoulBundle.h"
    "${CODE}/GhoulDemand.h"
    "${CODE}/GhoulFileTypes.h")
source_group("Header Files\\Rare Replay" FILES "${CODE}/RPK.h")
source_group("Header Files\\Xenon" FILES
    "${CODE}/XUIString.h"
    "${CODE}/XWaveBank.h"
    "${CODE}/xenon_texture.h")
source_group("Header Files\\Xbox" FILES "${CODE}/xbox_texture.h")
source_group("Header Files\\Pinata" FILES "${CODE}/PinataDBBundle.h")
source_group("Header Files\\Kameo" FILES "${CODE}/KameoDatabase.h")
source_group("Header Files\\Conker" FILES "${CODE}/ConkerFileTypes.h")
source_group("Header Files\\PDZ" FILES "${CODE}/DarkPackage.h")
source_group("Header Files\\Program Segments" FILES
    "${CODE}/LoadingProcess.h"
    "${CODE}/LoctextWindow.h"
    "${CODE}/MarkerWindow.h"
    "${CODE}/MessageHandle.h"
    "${CODE}/ScriptWindow.h"
    "${CODE}/VehicleWindow.h"
    "${CODE}/Program Segments/BundleSeg.h")

source_group("Resource Files" FILES MumboGarage.rc "${RES}/exeIcon.ico")
source_group("Resource Files\\images" FILES ${RES_IMAGES})
source_group("Resource Files\\fonts" FILES ${RES_FONTS})
