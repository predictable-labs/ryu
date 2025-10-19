add_library(ryu_processor_operator_parquet_writer
        OBJECT
        basic_column_writer.cpp
        boolean_column_writer.cpp
        column_writer.cpp
        interval_column_writer.cpp
        struct_column_writer.cpp
        string_column_writer.cpp
        list_column_writer.cpp
        parquet_writer.cpp
        parquet_rle_bp_encoder.cpp
        uuid_column_writer.cpp)

set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:ryu_processor_operator_parquet_writer>
        PARENT_SCOPE)

target_link_libraries(ryu_processor_operator_parquet_writer PUBLIC parquet)
