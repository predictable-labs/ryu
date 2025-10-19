#include "processor/operator/empty_result.h"

namespace ryu {
namespace processor {

bool EmptyResult::getNextTuplesInternal(ExecutionContext*) {
    return false;
}

} // namespace processor
} // namespace ryu
