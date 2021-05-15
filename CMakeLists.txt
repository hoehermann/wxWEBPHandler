# for testing purposes, this compiles the handler into a static library
cmake_minimum_required(VERSION 3.19) # TODO: find minimal version
set(PROJECT_NAME wxWEBPHandler)
project(${PROJECT_NAME})
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})
add_library(${PROJECT_NAME} STATIC
src/common/imagwebp.cpp
include/wx/imagwebp.hpp
check_directives.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE include) # find own includes (structure follows wxWidgets' conventions)
target_compile_definitions(${PROJECT_NAME} PRIVATE wxUSE_IMAGE wxUSE_STREAMS wxUSE_WEBP) # force definitions for stand-alone build (code follows wxWidgets' conventions)
find_package(WebP CONFIG)
if (${WebP_FOUND})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES} WebP::webpdecoder)
else()
    message(STATUS "WebP: Falling back to prefix search…")
    find_library(WebP_LIBRARIES webp)
    message(STATUS "WebP_LIBRARIES: ${WebP_LIBRARIES}")
    find_path(WebP_INCLUDE_DIRS NAMES "webp/decode.h") 
    message(STATUS "WebP_INCLUDE_DIRS: ${WebP_INCLUDE_DIRS}")
    target_include_directories(${PROJECT_NAME} PRIVATE ${WebP_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES} ${WebP_LIBRARIES})
endif()
