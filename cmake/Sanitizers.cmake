function(http_server_enable_sanitizers target_name)
    if(NOT HTTP_ENABLE_SANITIZERS)
        return()
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target_name}
            PUBLIC
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
        )

        target_link_options(${target_name}
            PUBLIC
                -fsanitize=address,undefined
        )
    endif()
endfunction()
