cmake_minimum_required(VERSION 3.28)

project(
    modern_http_server
    VERSION 0.1.0
    LANGUAGES CXX
)

option(HTTP_BUILD_TESTS "Build tests" ON)
option(HTTP_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(HTTP_ENABLE_WERROR "Treat warnings as errors" OFF)
option(HTTP_ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(HTTP_ENABLE_LTO "Enable link-time optimization" OFF)

# Keep warning and sanitizer setup in small helper files so targets stay readable.
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)

# Core server code lives in a library so the app, tests, and future benchmarks
# can all link the same implementation.
add_library(server_core
    src/platform/fd.cpp
    src/net/endpoint.cpp
    src/net/tcp_listener.cpp
    src/net/tcp_connection.cpp

    src/http/request.cpp
    src/http/response.cpp
    src/http/parser.cpp
    src/http/request_grammar.cpp
    src/http/router.cpp
)

target_compile_features(server_core PUBLIC cxx_std_23)

set_target_properties(server_core PROPERTIES
    CXX_EXTENSIONS OFF
)

target_include_directories(server_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

http_server_set_project_warnings(server_core)
http_server_enable_sanitizers(server_core)

if(HTTP_ENABLE_LTO)
    # LTO support depends on the compiler and linker, so check before enabling it.
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

    if(ipo_supported)
        set_property(TARGET server_core PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "IPO/LTO not supported: ${ipo_error}")
    endif()
endif()

add_executable(http_server
    src/main.cpp
)

target_link_libraries(http_server
    PRIVATE
        server_core
)

if(HTTP_BUILD_TESTS)
    enable_testing()

    find_package(Catch2 3 REQUIRED)
    include(Catch)

    add_subdirectory(tests/unit)
    add_subdirectory(tests/integration)
endif()

# Benchmarks are kept out of normal builds until the benchmark targets exist.
if(HTTP_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
