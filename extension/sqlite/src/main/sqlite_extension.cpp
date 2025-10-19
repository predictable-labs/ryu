#include "main/sqlite_extension.h"

#include "main/client_context.h"
#include "main/database.h"
#include "storage/sqlite_storage.h"

namespace ryu {
namespace sqlite_extension {

void SqliteExtension::load(main::ClientContext* context) {
    auto db = context->getDatabase();
    db->registerStorageExtension(EXTENSION_NAME, std::make_unique<SqliteStorageExtension>(*db));
    db->addExtensionOption("sqlite_all_varchar", common::LogicalTypeID::BOOL, common::Value{false});
}

} // namespace sqlite_extension
} // namespace ryu

#if defined(BUILD_DYNAMIC_LOAD)
extern "C" {
// Because we link against the static library on windows, we implicitly inherit RYU_STATIC_DEFINE,
// which cancels out any exporting, so we can't use RYU_API.
#if defined(_WIN32)
#define INIT_EXPORT __declspec(dllexport)
#else
#define INIT_EXPORT __attribute__((visibility("default")))
#endif
INIT_EXPORT void init(ryu::main::ClientContext* context) {
    ryu::sqlite_extension::SqliteExtension::load(context);
}

INIT_EXPORT const char* name() {
    return ryu::sqlite_extension::SqliteExtension::EXTENSION_NAME;
}
}
#endif
