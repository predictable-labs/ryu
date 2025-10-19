#include "processor/result/flat_tuple.h"

#include "c_api/helpers.h"
#include "c_api/ryu.h"
#include "common/exception/exception.h"

using namespace ryu::common;
using namespace ryu::processor;

void ryu_flat_tuple_destroy(ryu_flat_tuple* flat_tuple) {
    if (flat_tuple == nullptr) {
        return;
    }
    if (flat_tuple->_flat_tuple != nullptr && !flat_tuple->_is_owned_by_cpp) {
        delete static_cast<FlatTuple*>(flat_tuple->_flat_tuple);
    }
}

ryu_state ryu_flat_tuple_get_value(ryu_flat_tuple* flat_tuple, uint64_t index,
    ryu_value* out_value) {
    auto flat_tuple_ptr = static_cast<FlatTuple*>(flat_tuple->_flat_tuple);
    Value* _value = nullptr;
    try {
        _value = flat_tuple_ptr->getValue(index);
    } catch (Exception& e) {
        return RyuError;
    }
    out_value->_value = _value;
    // We set the ownership of the value to C++, so it will not be deleted if the value is destroyed
    // in C.
    out_value->_is_owned_by_cpp = true;
    return RyuSuccess;
}

char* ryu_flat_tuple_to_string(ryu_flat_tuple* flat_tuple) {
    auto flat_tuple_ptr = static_cast<FlatTuple*>(flat_tuple->_flat_tuple);
    return convertToOwnedCString(flat_tuple_ptr->toString());
}
