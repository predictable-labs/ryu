#include "connector/duckdb_secret_manager.h"

#include "s3fs_config.h"

namespace ryu {
namespace duckdb_extension {

static std::string getDuckDBExtensionOptions(httpfs_extension::S3AuthParams ryuOptions) {
    std::string options = "";
    options.append(common::stringFormat("KEY_ID '{}',", ryuOptions.accessKeyID));
    options.append(common::stringFormat("SECRET '{}',", ryuOptions.secretAccessKey));
    options.append(common::stringFormat("ENDPOINT '{}',", ryuOptions.endpoint));
    options.append(common::stringFormat("URL_STYLE '{}',", ryuOptions.urlStyle));
    options.append(common::stringFormat("REGION '{}',", ryuOptions.region));
    return options;
}

std::string DuckDBSecretManager::getRemoteS3FSSecret(main::ClientContext* context,
    const httpfs_extension::S3FileSystemConfig& config) {
    KU_ASSERT(config.fsName == "S3" || config.fsName == "GCS");
    std::string templateQuery = R"(CREATE SECRET {}_secret (
        {}
        TYPE {}
    );)";
    return common::stringFormat(templateQuery, config.fsName,
        getDuckDBExtensionOptions(config.getAuthParams(context)), config.fsName);
}

} // namespace duckdb_extension
} // namespace ryu
