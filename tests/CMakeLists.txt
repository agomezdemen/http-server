find_package(Catch2 3 REQUIRED)

# All test executables link the server core and use Catch2's generated main.
function(http_server_add_catch_test target_name)
    add_executable(${target_name}
        ${ARGN}
    )

    target_link_libraries(${target_name}
        PRIVATE
            http::server_core
            Catch2::Catch2WithMain
    )
endfunction()

http_server_add_catch_test(fd_tests fd_tests.cpp)
http_server_add_catch_test(endpoint_tests endpoint_tests.cpp)
http_server_add_catch_test(tcp_listener_tests tcp_listener_tests.cpp)
http_server_add_catch_test(tcp_connection_tests tcp_connection_tests.cpp)
http_server_add_catch_test(response_tests response_tests.cpp)
http_server_add_catch_test(request_tests request_tests.cpp)
http_server_add_catch_test(grammar_tests grammar_tests.cpp)

include(Catch)

# Discover Catch2 test cases at test time, after the executables have been built.
catch_discover_tests(fd_tests
    DISCOVERY_MODE PRE_TEST
)
catch_discover_tests(endpoint_tests
    DISCOVERY_MODE PRE_TEST
)
catch_discover_tests(tcp_listener_tests
    DISCOVERY_MODE PRE_TEST
)
catch_discover_tests(tcp_connection_tests
    DISCOVERY_MODE PRE_TEST
)
catch_discover_tests(response_tests
    DISCOVERY_MODE PRE_TEST
)

catch_discover_tests(request_tests
  DISCOVERY_MODE PRE_TEST
)

catch_discover_tests(grammar_tests
  DISCOVERY_MODE PRE_TEST
)
