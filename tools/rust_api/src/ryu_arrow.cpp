#include "ryu_arrow.h"

namespace ryu_arrow {

ArrowSchema query_result_get_arrow_schema(const ryu::main::QueryResult& result) {
    // Could use directly, except that we can't (yet) mark ArrowSchema as being safe to store in a
    // cxx::UniquePtr
    return *result.getArrowSchema();
}

ArrowArray query_result_get_next_arrow_chunk(ryu::main::QueryResult& result, uint64_t chunkSize) {
    return *result.getNextArrowChunk(chunkSize);
}

} // namespace ryu_arrow
