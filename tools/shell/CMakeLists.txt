include_directories(
        include
        ${PROJECT_SOURCE_DIR}/third_party/taywee_args/include
        ${PROJECT_SOURCE_DIR}/third_party/yyjson/src
        ${PROJECT_SOURCE_DIR}/extension/json/src/include
)

add_subdirectory(printer)

add_executable(ryu_shell
        ${RYU_SHELL_OBJECT_FILES}
        embedded_shell.cpp
        linenoise.cpp
        shell_runner.cpp)

if (NOT MSVC)
    # On Windows, rename the executable to ryu.exe will cause an error
    # "multiple rules generate src/ryu.lib", so we do not set the output
    # name for MSVC.
    set_target_properties(ryu_shell PROPERTIES OUTPUT_NAME ryu)
endif ()


target_link_libraries(ryu_shell ryu)
if (ENABLE_BACKTRACES)
    target_link_libraries(ryu_shell register_backtrace_signal_handler)
endif ()
if (MSVC)
    target_link_libraries(ryu_shell ws2_32)
endif ()

install(TARGETS ryu_shell)
