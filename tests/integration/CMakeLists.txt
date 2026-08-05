add_executable(http_server_integration_tests
    http_server_tests.cpp
    server_process.cpp
    http_client.cpp
)

target_compile_features(http_server_integration_tests
    PRIVATE
        cxx_std_23
)

target_link_libraries(http_server_integration_tests
    PRIVATE
        Catch2::Catch2WithMain
)

target_compile_definitions(http_server_integration_tests
    PRIVATE
        HTTP_SERVER_EXECUTABLE="$<TARGET_FILE:http_server>"
)

add_dependencies(http_server_integration_tests
    http_server
)

http_server_set_project_warnings(http_server_integration_tests)
http_server_enable_sanitizers(http_server_integration_tests)

catch_discover_tests(http_server_integration_tests
    DISCOVERY_MODE PRE_TEST
    TEST_PREFIX "integration: "
    PROPERTIES
        TIMEOUT 5
        RUN_SERIAL TRUE
)
