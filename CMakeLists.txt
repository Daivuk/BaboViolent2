cmake_minimum_required(VERSION 3.0.0)

# Define LINUX
if (UNIX AND NOT APPLE)
    set(LINUX 1)
endif()

# Setup modules path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Project name
project(BaboViolent)

# Some compiler flags
set(CMAKE_CXX_STANDARD 11) # C++11
if (MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "/MP") # Multi core in VS
endif()

# Define _DEBUG
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif()

#justwindowsthings
if (WIN32)
    add_definitions(-DNOMINMAX)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

# PRO version
add_definitions(-D_PRO_)
add_definitions(-D_MINIBOT_)

# Project files
file(GLOB src_Engine_Babonet ./src/Engine/Babonet/*.*)
source_group("Engine\\Babonet" FILES ${src_Engine_Babonet})
file(GLOB src_Engine_dko ./src/Engine/dko/*.*)
source_group("Engine\\dko" FILES ${src_Engine_dko})
file(GLOB src_Engine_Zeven_dkc ./src/Engine/Zeven/dkc/*.*)
source_group("Engine\\Zeven\\dkc" FILES ${src_Engine_Zeven_dkc})
file(GLOB src_Engine_Zeven_dkf ./src/Engine/Zeven/dkf/*.*)
source_group("Engine\\Zeven\\dkf" FILES ${src_Engine_Zeven_dkf})
file(GLOB src_Engine_Zeven_dkgl ./src/Engine/Zeven/dkgl/*.*)
source_group("Engine\\Zeven\\dkgl" FILES ${src_Engine_Zeven_dkgl})
file(GLOB src_Engine_Zeven_dki ./src/Engine/Zeven/dki/*.*)
source_group("Engine\\Zeven\\dki" FILES ${src_Engine_Zeven_dki})
file(GLOB src_Engine_Zeven_dkp ./src/Engine/Zeven/dkp/*.*)
source_group("Engine\\Zeven\\dkp" FILES ${src_Engine_Zeven_dkp})
file(GLOB src_Engine_Zeven_dks ./src/Engine/Zeven/dks/*.*)
source_group("Engine\\Zeven\\dks" FILES ${src_Engine_Zeven_dks})
file(GLOB src_Engine_Zeven_dksvar ./src/Engine/Zeven/dksvar/*.*)
source_group("Engine\\Zeven\\dksvar" FILES ${src_Engine_Zeven_dksvar})
file(GLOB src_Engine_Zeven_dkt ./src/Engine/Zeven/dkt/*.*)
source_group("Engine\\Zeven\\dkt" FILES ${src_Engine_Zeven_dkt})
file(GLOB src_Engine_Zeven_dkw ./src/Engine/Zeven/dkw/*.*)
source_group("Engine\\Zeven\\dkw" FILES ${src_Engine_Zeven_dkw})
file(GLOB src_External ./src/External/*.*)
source_group("External" FILES ${src_External})
file(GLOB src_Game ./src/Game/*.*)
source_group("Game" FILES ${src_Game})
file(GLOB src_Game_AStar ./src/Game/AStar/*.*)
source_group("Game\\AStar" FILES ${src_Game_AStar})
file(GLOB src_Game_Master ./src/Game/Master/*.*)
source_group("Game\\Master" FILES ${src_Game_Master})
file(GLOB src_Menu ./src/Menu/*.*)
source_group("Menu" FILES ${src_Menu})
file(GLOB src_Menu2 ./src/Menu2/*.*)
source_group("Menu2" FILES ${src_Menu2})
file(GLOB src_Menu2_Dialogs ./src/Menu2/Dialogs/*.*)
source_group("Menu2\\Dialogs" FILES ${src_Menu2_Dialogs})
file(GLOB src_Source ./src/Source/*.*)
source_group("Source" FILES ${src_Source})
file(GLOB src_Weather ./src/Weather/*.*)
source_group("Weather" FILES ${src_Weather})
file(GLOB src_Zeven ./src/Zeven/*.*)
source_group("Zeven" FILES ${src_Zeven})
file(GLOB src_Zeven_API ./src/Zeven/API/*.*)
source_group("Zeven\\API" FILES ${src_Zeven_API})

list(APPEND includes PUBLIC
    ./src/Engine/Babonet/
    ./src/Engine/dko/
    ./src/Engine/Zeven/dkc/
    ./src/Engine/Zeven/dkf/
    ./src/Engine/Zeven/dkgl/
    ./src/Engine/Zeven/dki/
    ./src/Engine/Zeven/dkp/
    ./src/Engine/Zeven/dks/
    ./src/Engine/Zeven/dksvar/
    ./src/Engine/Zeven/dkt/
    ./src/Engine/Zeven/dkw/
    ./src/External/
    ./src/Game/
    ./src/Game/AStar/
    ./src/Game/Master/
    ./src/Menu/
    ./src/Menu2/
    ./src/Menu2/Dialogs/
    ./src/Source/
    ./src/Weather/
    ./src/Zeven/
    ./src/Zeven/API/
)

# cURL
option(BUILD_SHARED_LIBS OFF)
option(HTTP_ONLY ON)
set(BUILD_TESTING OFF)
option(BUILD_CURL_EXE OFF)
add_subdirectory(./thirdparty/curl/)
list(APPEND includes ./thirdparty/curl/include/)
list(APPEND libs libcurl)

# sqlite3
file(GLOB src_thirdparty_sqlite ./thirdparty/sqlite/*.*)
source_group("thirdparty\\sqlite" FILES ${src_thirdparty_sqlite})
list(APPEND includes ./thirdparty/sqlite/)

# SDL
set(HAVE_LIBC ON)
add_subdirectory(./thirdparty/SDL/)
list(APPEND libs SDL2-static)
list(APPEND includes PUBLIC ./thirdparty/SDL/include/)

# OpenGL
find_package(OpenGL REQUIRED)
list(APPEND includes PUBLIC ${OPENGL_INCLUDE_DIR})
list(APPEND libs ${OPENGL_LIBRARIES})

# Dear imgui
list(APPEND src_thirdparty_imgui
    ./thirdparty/imgui/imgui_demo.cpp
    ./thirdparty/imgui/imgui_draw.cpp
    ./thirdparty/imgui/imgui_internal.h
    ./thirdparty/imgui/imgui.cpp
    ./thirdparty/imgui/imgui.h
    ./thirdparty/imgui/stb_rect_pack.h
    ./thirdparty/imgui/stb_textedit.h
    ./thirdparty/imgui/stb_truetype.h
    ./thirdparty/imgui/examples/imgui_impl_opengl3.cpp
    ./thirdparty/imgui/examples/imgui_impl_opengl3.h
    ./thirdparty/imgui/examples/imgui_impl_sdl.cpp
    ./thirdparty/imgui/examples/imgui_impl_sdl.h
)
source_group("thirdparty\\imgui" FILES ${src_thirdparty_imgui})
list(APPEND includes PUBLIC ./thirdparty/imgui/ ./thirdparty/imgui/examples/)

# BrebisGL
list(APPEND includes PUBLIC ./thirdparty/BrebisGL/)
file(GLOB src_thirdparty_brebisgl ./thirdparty/BrebisGL/*.*)

# BaboViolent.exe, use WinMain on Windows
add_executable(BaboViolent WIN32
    ${src_Engine_Babonet}
    ${src_Engine_dko}
    ${src_Engine_Zeven_dkc}
    ${src_Engine_Zeven_dkf}
    ${src_Engine_Zeven_dkgl}
    ${src_Engine_Zeven_dki}
    ${src_Engine_Zeven_dkp}
    ${src_Engine_Zeven_dks}
    ${src_Engine_Zeven_dksvar}
    ${src_Engine_Zeven_dkt}
    ${src_Engine_Zeven_dkw}
    ${src_External}
    ${src_Game}
    ${src_Game_AStar}
    ${src_Game_Master}
    ${src_Menu}
    ${src_Menu2}
    ${src_Menu2_Dialogs}
    ${src_Source}
    ${src_Weather}
    ${src_Zeven}
    ${src_Zeven_API}

    ${src_thirdparty_sqlite}
    ${src_thirdparty_imgui}
    ${src_thirdparty_brebisgl}
)

# Lib/Headers
target_include_directories(BaboViolent ${includes})
target_link_libraries(BaboViolent ${libs})