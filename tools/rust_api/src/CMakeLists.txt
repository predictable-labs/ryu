add_library(ryu_rs STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ryu_rs.cpp
    ${RYU_RS_BINDINGS_DIR}/sources/ryu/src/lib.rs.cc
)

target_include_directories(ryu_rs
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${RYU_RS_BINDINGS_DIR}/include
        ${RYU_RS_BINDINGS_DIR}/crate
        ${RYU_RS_BINDINGS_DIR}/sources
)

target_link_libraries(ryu_rs ryu)
