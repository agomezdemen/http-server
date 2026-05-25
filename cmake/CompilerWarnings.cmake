function(http_server_set_project_warnings target_name)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target_name}
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
            target_compile_options(${target_name} PRIVATE -Werror)
        endif()
    endif()
endfunction()
