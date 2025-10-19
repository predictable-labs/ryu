#pragma once

#include "extension/extension.h"

namespace ryu {
namespace iceberg_extension {

class IcebergExtension final : public extension::Extension {
public:
    static constexpr char EXTENSION_NAME[] = "ICEBERG";

public:
    static void load(main::ClientContext* context);
};

} // namespace iceberg_extension
} // namespace ryu
