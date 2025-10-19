add_subdirectory(map)
add_subdirectory(operator)
add_subdirectory(result)

add_library(ryu_processor
        OBJECT
        warning_context.cpp
        processor.cpp
        processor_task.cpp)

set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:ryu_processor>
        PARENT_SCOPE)
