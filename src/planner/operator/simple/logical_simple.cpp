#include "planner/operator/simple/logical_simple.h"

namespace ryu {
namespace planner {

void LogicalSimple::computeFlatSchema() {
    createEmptySchema();
}

void LogicalSimple::computeFactorizedSchema() {
    createEmptySchema();
}

} // namespace planner
} // namespace ryu
