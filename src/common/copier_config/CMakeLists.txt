add_library(ryu_common_copier_config
        OBJECT
        csv_reader_config.cpp
        reader_config.cpp)

set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:ryu_common_copier_config>
        PARENT_SCOPE)
