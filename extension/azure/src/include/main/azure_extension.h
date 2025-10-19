#pragma once

#include "extension/extension.h"

namespace ryu {
namespace azure_extension {

class AzureExtension final : public extension::Extension {
public:
    static constexpr char EXTENSION_NAME[] = "AZURE";

public:
    static void load(main::ClientContext* context);
};

} // namespace azure_extension
} // namespace ryu
