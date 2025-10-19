#pragma once

#include "catalog/catalog.h"

namespace ryu {
namespace extension {

class RYU_API CatalogExtension : public catalog::Catalog {
public:
    CatalogExtension() : Catalog() {}

    virtual void init() = 0;

    void invalidateCache();
};

} // namespace extension
} // namespace ryu
