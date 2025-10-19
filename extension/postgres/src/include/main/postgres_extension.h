#pragma once

#include "extension/extension.h"

namespace ryu {
namespace postgres_extension {

class PostgresExtension final : public extension::Extension {
public:
    static constexpr char EXTENSION_NAME[] = "POSTGRES";

public:
    static void load(main::ClientContext* context);
};

} // namespace postgres_extension
} // namespace ryu
