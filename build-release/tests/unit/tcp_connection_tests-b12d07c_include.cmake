if(EXISTS "/home/bean/coding/cpp/http-server/build-release/tests/unit/tcp_connection_tests")
  if(NOT EXISTS "/home/bean/coding/cpp/http-server/build-release/tests/unit/tcp_connection_tests-b12d07c_tests.cmake" OR
     NOT "/home/bean/coding/cpp/http-server/build-release/tests/unit/tcp_connection_tests-b12d07c_tests.cmake" IS_NEWER_THAN "/home/bean/coding/cpp/http-server/build-release/tests/unit/tcp_connection_tests" OR
     NOT "/home/bean/coding/cpp/http-server/build-release/tests/unit/tcp_connection_tests-b12d07c_tests.cmake" IS_NEWER_THAN "${CMAKE_CURRENT_LIST_FILE}")
    include("/usr/lib/cmake/Catch2/CatchAddTests.cmake")
    catch_discover_tests_impl(
      TEST_EXECUTABLE [==[/home/bean/coding/cpp/http-server/build-release/tests/unit/tcp_connection_tests]==]
      TEST_EXECUTOR [==[]==]
      TEST_WORKING_DIR [==[/home/bean/coding/cpp/http-server/build-release/tests/unit]==]
      TEST_SPEC [==[]==]
      TEST_EXTRA_ARGS [==[]==]
      TEST_PROPERTIES [==[SKIP_RETURN_CODE;4]==]
      TEST_PREFIX [==[]==]
      TEST_SUFFIX [==[]==]
      TEST_LIST [==[tcp_connection_tests_TESTS]==]
      TEST_REPORTER [==[]==]
      TEST_OUTPUT_DIR [==[]==]
      TEST_OUTPUT_PREFIX [==[]==]
      TEST_OUTPUT_SUFFIX [==[]==]
      CTEST_FILE [==[/home/bean/coding/cpp/http-server/build-release/tests/unit/tcp_connection_tests-b12d07c_tests.cmake]==]
      TEST_DL_PATHS [==[]==]
      TEST_DL_FRAMEWORK_PATHS [==[]==]
      ADD_TAGS_AS_LABELS [==[FALSE]==]
    )
  endif()
  include("/home/bean/coding/cpp/http-server/build-release/tests/unit/tcp_connection_tests-b12d07c_tests.cmake")
else()
  add_test(tcp_connection_tests_NOT_BUILT tcp_connection_tests_NOT_BUILT)
endif()
