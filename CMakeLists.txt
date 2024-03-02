# for testing purposes, this compiles the handler into a static library
cmake_minimum_required(VERSION 3.16) # TODO: find minimal version
set(PROJECT_NAME wxWEBPHandler)
project(${PROJECT_NAME})
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})
add_library(${PROJECT_NAME} STATIC
    src/common/imagwebp.cpp
    include/wx/imagwebp.h
)
target_include_directories(${PROJECT_NAME} PRIVATE include) # find own includes (structure follows wxWidgets' conventions)
target_compile_definitions(${PROJECT_NAME} PRIVATE wxUSE_IMAGE wxUSE_STREAMS wxUSE_WEBP wxOVERRIDE=) # force definitions for stand-alone build (code follows wxWidgets' conventions)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
find_package(WebP CONFIG)
if (${WebP_FOUND})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES})
else()
    message(STATUS "WebP: Falling back to prefix search…")
    find_library(WebP_LIBRARIES_webp webp)
    find_library(WebP_LIBRARIES_demux webpdemux)
    set(WebP_LIBRARIES ${WebP_LIBRARIES_webp} ${WebP_LIBRARIES_demux})
    message(STATUS "WebP_LIBRARIES: ${WebP_LIBRARIES}")
    find_path(WebP_INCLUDE_DIRS NAMES "webp/decode.h") 
    message(STATUS "WebP_INCLUDE_DIRS: ${WebP_INCLUDE_DIRS}")
    target_include_directories(${PROJECT_NAME} PRIVATE ${WebP_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES} ${WebP_LIBRARIES})
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_executable(wxWEBPHandlerTest
        test/imagwebptest.cpp
        test/check_directives.cpp
    )
    target_include_directories(wxWEBPHandlerTest PRIVATE include)
    target_compile_definitions(wxWEBPHandlerTest PRIVATE wxUSE_IMAGE wxUSE_STREAMS wxUSE_WEBP wxOVERRIDE=)
    target_include_directories(wxWEBPHandlerTest PRIVATE ${WebP_INCLUDE_DIRS})
    target_link_libraries(wxWEBPHandlerTest PRIVATE ${wxWidgets_LIBRARIES} ${PROJECT_NAME})
endif()
