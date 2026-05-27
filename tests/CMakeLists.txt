find_package(Catch2 3 REQUIRED)

add_executable(fd_tests
    fd_tests.cpp
)

target_link_libraries(fd_tests
    PRIVATE
        http::server_core
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(fd_tests
    DISCOVERY_MODE PRE_TEST
)
