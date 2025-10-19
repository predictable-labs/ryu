#include "loader/duckdb_loader.h"

#include "main/duckdb_extension.h"

namespace ryu {
namespace duckdb_extension {

void DuckDBLoader::loadDependency(main::ClientContext* context) {
    for (auto& dependencyLib : DuckdbExtension::DEPENDENCY_LIB_FILES) {
        auto dependencyLibWithSuffix = extension::ExtensionUtils::appendLibSuffix(dependencyLib);
        auto dependencyLibPath =
            extension::ExtensionUtils::getLocalPathForSharedLib(context, dependencyLibWithSuffix);
        auto dependencyLoader = extension::ExtensionLibLoader(extensionName, dependencyLibPath);
    }
}

} // namespace duckdb_extension
} // namespace ryu

extern "C" {
// Because we link against the static library on windows, we implicitly inherit RYU_STATIC_DEFINE,
// which cancels out any exporting, so we can't use RYU_API.
#if defined(_WIN32)
#define INIT_EXPORT __declspec(dllexport)
#else
#define INIT_EXPORT __attribute__((visibility("default")))
#endif
INIT_EXPORT void load(ryu::main::ClientContext* context) {
    ryu::duckdb_extension::DuckDBLoader loader{"duckdb"};
    loader.loadDependency(context);
}
}
