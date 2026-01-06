#include <openspm.hpp>
#include <yaml-cpp/yaml.h>
std::string openspm_repo::generatePackageMetadata(std::vector<PackageInfo> &packages)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "packages" << YAML::Value << YAML::BeginSeq;
    for (const auto &pkg : packages)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "name" << YAML::Value << pkg.name;
        out << YAML::Key << "version" << YAML::Value << pkg.version;
        out << YAML::Key << "description" << YAML::Value << pkg.description;
        out << YAML::Key << "maintainer" << YAML::Value << pkg.maintainer;
        out << YAML::Key << "dependencies" << YAML::Value << YAML::BeginSeq;
        for (const auto &dep : pkg.dependencies)
        {
            out << dep;
        }
        out << YAML::EndSeq;
        out << YAML::Key << "tags" << YAML::Value << pkg.tags;
        out << YAML::Key << "url" << YAML::Value << pkg.url;
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;
    return std::string(out.c_str());
}
openspm_repo::PackageInfo openspm_repo::parsePackageYaml(const std::string &yamlContent)
{
    PackageInfo pkg;
    YAML::Node node = YAML::Load(yamlContent);
    pkg.name = node["name"].as<std::string>();
    pkg.version = node["version"].as<std::string>();
    pkg.description = node["description"].as<std::string>();
    pkg.maintainer = node["maintainer"].as<std::string>();
    for (const auto &dep : node["dependencies"])
    {
        pkg.dependencies.push_back(dep.as<std::string>());
    }
    pkg.tags = node["tags"].as<std::string>();
    pkg.url = node["url"].as<std::string>();
    return pkg;
}
std::string openspm_repo::generateRepositoryMetadata(const RepositoryInfo &repoInfo)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "url" << YAML::Value << repoInfo.url;
    out << YAML::Key << "name" << YAML::Value << repoInfo.name;
    out << YAML::Key << "description" << YAML::Value << repoInfo.description;
    out << YAML::Key << "maintainer" << YAML::Value << repoInfo.mantainer;
    out << YAML::EndMap;
    return std::string(out.c_str());
}
std::vector<openspm_repo::PackageInfo> openspm_repo::parsePackageMetadata(const std::string &metadataContent)
{
    std::vector<openspm_repo::PackageInfo> packages;
    YAML::Node doc = YAML::Load(metadataContent);
    for (const auto &node : doc)
    {
        PackageInfo pkg;
        pkg.name = node["name"].as<std::string>();
        pkg.version = node["version"].as<std::string>();
        pkg.description = node["description"].as<std::string>();
        pkg.maintainer = node["maintainer"].as<std::string>();
        for (const auto &dep : node["dependencies"])
        {
            pkg.dependencies.push_back(dep.as<std::string>());
        }
        pkg.tags = node["tags"].as<std::string>();
        pkg.url = node["url"].as<std::string>();
        packages.push_back(pkg);
    }

    return packages;
}
openspm_repo::RepositoryInfo openspm_repo::parseRepositoryMetadata(const std::string &metadataContent)
{
    RepositoryInfo repoInfo;
    YAML::Node doc = YAML::Load(metadataContent);
    repoInfo.url = doc["url"].as<std::string>();
    repoInfo.name = doc["name"].as<std::string>();
    repoInfo.description = doc["description"].as<std::string>();
    repoInfo.mantainer = doc["maintainer"].as<std::string>();
    return repoInfo;
}