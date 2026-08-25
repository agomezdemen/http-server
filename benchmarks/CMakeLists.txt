find_package(nlohmann_json REQUIRED)

add_executable(http_server_benchmarks
    main.cpp
    core/statistics.cpp
    http/request_parser_benchmark.cpp
)

target_compile_definitions(http_server_benchmarks
    PRIVATE
        HTTP_SERVER_ROOT="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(http_server_benchmarks
    PRIVATE
        server_core
        nlohmann_json::nlohmann_json
)

target_include_directories(http_server_benchmarks
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(http_server_benchmarks PRIVATE cxx_std_23)

set_target_properties(http_server_benchmarks PROPERTIES
    CXX_EXTENSIONS OFF
)

http_server_set_project_warnings(http_server_benchmarks)
http_server_enable_sanitizers(http_server_benchmarks)
