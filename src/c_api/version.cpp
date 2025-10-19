#include "main/version.h"

#include "c_api/helpers.h"
#include "c_api/ryu.h" // IWYU pragma: keep - Declares API functions implemented here

char* ryu_get_version() {
    return convertToOwnedCString(ryu::main::Version::getVersion());
}

uint64_t ryu_get_storage_version() {
    return ryu::main::Version::getStorageVersion();
}
