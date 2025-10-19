add_library(ryu_processor_result
        OBJECT
        base_hash_table.cpp
        factorized_table.cpp
        factorized_table_pool.cpp
        factorized_table_schema.cpp
        factorized_table_util.cpp
        flat_tuple.cpp
        pattern_creation_info_table.cpp
        result_set.cpp
        result_set_descriptor.cpp
        )

set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:ryu_processor_result>
        PARENT_SCOPE)
