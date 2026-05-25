add_executable(buffer_tests buffer_tests.cpp)
target_link_libraries(buffer_tests PRIVATE http::server_core)
add_test(NAME buffer_tests COMMAND buffer_tests)

add_executable(parser_tests parser_tests.cpp)
target_link_libraries(parser_tests PRIVATE http::server_core)
add_test(NAME parser_tests COMMAND parser_tests)

add_executable(response_tests response_tests.cpp)
target_link_libraries(response_tests PRIVATE http::server_core)
add_test(NAME response_tests COMMAND response_tests)
