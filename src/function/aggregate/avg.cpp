#include "function/aggregate/avg.h"

namespace ryu {
namespace function {

using namespace ryu::common;

function_set AggregateAvgFunction::getFunctionSet() {
    function_set result;
    for (auto typeID : LogicalTypeUtils::getNumericalLogicalTypeIDs()) {
        AggregateFunctionUtils::appendSumOrAvgFuncs<AvgFunction>(name, typeID, result);
    }
    return result;
}

} // namespace function
} // namespace ryu
