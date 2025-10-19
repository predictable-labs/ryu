#include "main/json_extension.h"

#include "json_cast_functions.h"
#include "json_creation_functions.h"
#include "json_export.h"
#include "json_extract_functions.h"
#include "json_scalar_functions.h"
#include "json_scan.h"
#include "json_type.h"
#include "main/client_context.h"
#include "main/database.h"

namespace ryu {
namespace json_extension {

using namespace ryu::extension;

static void addJsonCreationFunction(main::Database& db) {
    ExtensionUtils::addScalarFunc<ToJsonFunction>(db);
    ExtensionUtils::addScalarFuncAlias<JsonQuoteFunction>(db);
    ExtensionUtils::addScalarFuncAlias<ArrayToJsonFunction>(db);
    ExtensionUtils::addScalarFuncAlias<RowToJsonFunction>(db);
    ExtensionUtils::addScalarFunc<CastToJsonFunction>(db);
    ExtensionUtils::addScalarFunc<JsonArrayFunction>(db);
    ExtensionUtils::addScalarFunc<JsonObjectFunction>(db);
    ExtensionUtils::addScalarFunc<JsonMergePatchFunction>(db);
}

static void addJsonExtractFunction(main::Database& db) {
    ExtensionUtils::addScalarFunc<JsonExtractFunction>(db);
}

static void addJsonScalarFunction(main::Database& db) {
    ExtensionUtils::addScalarFunc<JsonArrayLengthFunction>(db);
    ExtensionUtils::addScalarFunc<JsonContainsFunction>(db);
    ExtensionUtils::addScalarFunc<JsonKeysFunction>(db);
    ExtensionUtils::addScalarFunc<JsonStructureFunction>(db);
    ExtensionUtils::addScalarFunc<JsonValidFunction>(db);
    ExtensionUtils::addScalarFunc<MinifyJsonFunction>(db);
}

void JsonExtension::load(main::ClientContext* context) {
    auto& db = *context->getDatabase();
    db.getCatalog()->createType(&transaction::DUMMY_TRANSACTION, JSON_TYPE_NAME,
        JsonType::getJsonType());
    addJsonCreationFunction(db);
    addJsonExtractFunction(db);
    addJsonScalarFunction(db);
    ExtensionUtils::addExportFunc<JsonExportFunction>(db);
    ExtensionUtils::addTableFunc<JsonScan>(db);
}

} // namespace json_extension
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
    ryu::json_extension::JsonExtension::load(context);
}

INIT_EXPORT const char* name() {
    return ryu::json_extension::JsonExtension::EXTENSION_NAME;
}
}
#endif
