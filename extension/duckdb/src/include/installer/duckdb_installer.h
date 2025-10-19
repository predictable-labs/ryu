#pragma once

#include "extension/extension_installer.h"

namespace ryu {
namespace duckdb_extension {

class RYU_API DuckDBInstaller final : public extension::ExtensionInstaller {
public:
    DuckDBInstaller(const extension::InstallExtensionInfo& info, main::ClientContext& context)
        : ExtensionInstaller{info, context} {}

    bool install() override;
};

} // namespace duckdb_extension
} // namespace ryu
