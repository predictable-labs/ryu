if (${BUILD_EXTENSION_TESTS})
    add_ryu_test(fts_prepare_test prepare_test.cpp)
    add_ryu_test(fts_read_only_test read_only_test.cpp)
endif ()
