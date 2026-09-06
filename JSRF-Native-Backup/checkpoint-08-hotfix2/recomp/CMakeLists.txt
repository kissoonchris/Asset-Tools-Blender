cmake_minimum_required(VERSION 3.20)
project(jsrf_recomp C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT WIN32)
    message(FATAL_ERROR "JSRF xboxrecomp host currently targets Windows/MSVC")
endif()

add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)

set(XBOXRECOMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.tools/xboxrecomp" CACHE PATH
    "Path to the xboxrecomp toolkit root directory")

if(NOT EXISTS "${XBOXRECOMP_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "xboxrecomp was not found at ${XBOXRECOMP_DIR}. Run Build-JSRF-XboxRecomp.bat first or set XBOXRECOMP_DIR.")
endif()

add_subdirectory(${XBOXRECOMP_DIR} ${CMAKE_BINARY_DIR}/xboxrecomp)

file(GLOB RECOMP_GEN_SOURCES CONFIGURE_DEPENDS "src/recomp/gen/*.c")
if(NOT RECOMP_GEN_SOURCES)
    message(FATAL_ERROR
        "No lifted JSRF source exists under src/recomp/gen. Run the generation step before configuring this project.")
endif()

add_executable(${PROJECT_NAME}
    src/main.c
    src/recomp_manual.c
    ${RECOMP_GEN_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/recomp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/recomp/gen
    ${XBOXRECOMP_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE xboxrecomp)
target_link_libraries(${PROJECT_NAME} PRIVATE
    d3d11
    dxgi
    dxguid
    xinput
    winmm
    dbghelp
)

if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE /LARGEADDRESSAWARE)
    target_compile_options(${PROJECT_NAME} PRIVATE /bigobj)
endif()
