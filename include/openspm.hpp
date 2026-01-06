#pragma once
#include <string>
#include <vector>
namespace openspm_repo
{
    struct PackageInfo
    {
        std::string name;                      ///< Package name
        std::string version;                   ///< Package version
        std::string description;               ///< Package description
        std::string maintainer;                ///< Package maintainer
        std::vector<std::string> dependencies; ///< List of dependency package names
        std::string tags;                      ///< Semicolon-separated tags (e.g., "bin;linux-x86_64")
        std::string url;                       ///< Download URL for the package archive
    };
    struct RepositoryInfo
    {
        std::string url;         ///< Repository base URL
        std::string name;        ///< Repository name
        std::string description; ///< Repository description
        std::string mantainer;   ///< Repository maintainer (note: typo preserved for compatibility)
    };
    std::vector<PackageInfo> parsePackageMetadata(const std::string &metadataContent);
    PackageInfo parsePackageYaml(const std::string &yamlContent);
    std::string generatePackageMetadata(std::vector<PackageInfo> &packages);
    RepositoryInfo parseRepositoryMetadata(const std::string &metadataContent);
    std::string generateRepositoryMetadata(const RepositoryInfo &repoInfo);
}