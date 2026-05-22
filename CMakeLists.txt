cmake_minimum_required(VERSION 3.28)

project(
    modern_http_server
    VERSION 0.1.0
    LANGUAGES CXX
)

option(HTTP_BUILD_TESTS "Build tests" ON)
option(HTTP_ENABLE_WERROR "Treat warnings as errors" OFF)
option(HTTP_ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(HTTP_ENABLE_LTO "Enable link-time optimization" OFF)

add_library(http_server_core
    src/net/fd.cpp
    src/net/socket.cpp
    src/net/epoll.cpp

    src/http/request.cpp
    src/http/response.cpp
    src/http/parser.cpp

    src/server.cpp
)

add_library(http::server_core ALIAS http_server_core)

target_compile_features(http_server_core PUBLIC cxx_std_20)

set_target_properties(http_server_core PROPERTIES
    CXX_EXTENSIONS OFF
)

target_include_directories(http_server_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(http_server_core
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wshadow
            -Wold-style-cast
            -Wnon-virtual-dtor
            -Woverloaded-virtual
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
    )

    if(HTTP_ENABLE_WERROR)
        target_compile_options(http_server_core PRIVATE -Werror)
    endif()

    if(HTTP_ENABLE_SANITIZERS)
        target_compile_options(http_server_core
            PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
        )

        target_link_options(http_server_core
            PRIVATE
                -fsanitize=address,undefined
        )
    endif()
endif()

if(HTTP_ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

    if(ipo_supported)
        set_property(TARGET http_server_core PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "IPO/LTO not supported: ${ipo_error}")
    endif()
endif()

add_executable(http_server
    src/main.cpp
)

target_link_libraries(http_server
    PRIVATE
        http::server_core
)

if(HTTP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
