#include <openspm_repo.hpp>
#include <openspm.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <archive.h>
#include <archive_entry.h>
int openspm_repo::rebuildRepositoryIndex(const std::string &repoPath)
{
    std::filesystem::path repoDir(repoPath);
    if (!std::filesystem::exists(repoDir))
    {
        std::cerr << "Error: Repository path does not exist." << std::endl;
        return 1;
    }
    std::ifstream repoFileIn(repoDir / "repository.yaml");
    if (!repoFileIn.good())
    {
        std::cerr << "Error: Repository metadata file not found." << std::endl;
        return 1;
    }
    RepositoryInfo repoInfo;
    std::string repoMetadataContent((std::istreambuf_iterator<char>(repoFileIn)), std::istreambuf_iterator<char>());
    repoInfo = parseRepositoryMetadata(repoMetadataContent);
    repoFileIn.close();
    std::string repoBaseUrl = repoInfo.url;
    std::vector<std::filesystem::path> tarGzFiles;
    for (const auto &entry : std::filesystem::recursive_directory_iterator(repoDir))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".gz")
        {
            if (entry.path().stem().extension() == ".tar")
            {
                tarGzFiles.push_back(entry.path());
            }
        }
    }
    std::vector<PackageInfo> packages;
    for (const auto &tarGzPath : tarGzFiles)
    {
        PackageInfo pkgInfo;
        if (readPackageFromArchive(tarGzPath.string(), pkgInfo) == 0)
        {
            std::filesystem::path relativePath = std::filesystem::relative(tarGzPath, repoDir);
            std::string packageUrl = repoBaseUrl;
            if (!packageUrl.empty() && packageUrl.back() != '/')
            {
                packageUrl += "/";
            }
            packageUrl += relativePath.string();
            pkgInfo.url = packageUrl;

            packages.push_back(pkgInfo);
        }
    }
    std::string metadataContent = generatePackageMetadata(packages);
    std::ofstream indexFile(repoDir / "pkg-list.yaml");
    indexFile << metadataContent;
    indexFile.close();
    return 0;
}
int openspm_repo::readPackageFromArchive(const std::string &archivePath, PackageInfo &pkgInfo)
{
    struct archive *a = archive_read_new();
    archive_read_support_filter_gzip(a);
    archive_read_support_format_tar(a);

    if (archive_read_open_filename(a, archivePath.c_str(), 10240) != ARCHIVE_OK)
    {
        std::cerr << "Error: Failed to open archive " << archivePath << std::endl;
        archive_read_free(a);
        return 1;
    }

    struct archive_entry *entry;
    bool found = false;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    {
        std::string entryName = archive_entry_pathname(entry);
        std::string suffix = "/pkg.yaml";
        if (entryName == "pkg.yaml" || (entryName.size() >= suffix.size() && entryName.compare(entryName.size() - suffix.size(), suffix.size(), suffix) == 0))
        {
            size_t size = archive_entry_size(entry);
            std::string yamlContent(size, '\0');
            archive_read_data(a, yamlContent.data(), size);
            pkgInfo = openspm_repo::parsePackageYaml(yamlContent);
            found = true;
            break;
        }
        else
        {
            archive_read_data_skip(a);
        }
    }

    if (!found)
    {
        std::cerr << "Warning: pkg.yaml not found in " << archivePath << std::endl;
    }
    archive_read_free(a);
    return 0;
}
int openspm_repo::addUrlToRepository(const std::string &repoPath, const std::string &packagesUrl)
{
    std::filesystem::path repoDir(repoPath);
    if (!std::filesystem::exists(repoDir))
    {
        std::cerr << "Error: Repository path does not exist." << std::endl;
        return 1;
    }
    std::ifstream indexFileIn(repoDir / "pkg-list.yaml");
    std::string indexContent;
    if (indexFileIn.good())
    {
        indexContent.assign((std::istreambuf_iterator<char>(indexFileIn)), std::istreambuf_iterator<char>());
        indexFileIn.close();
    }
    YAML::Node indexNode;
    if (!indexContent.empty())
    {
        indexNode = YAML::Load(indexContent);
    }
    else
    {
        indexNode = YAML::Node(YAML::NodeType::Map);
    }

    if (!indexNode["depend"])
    {
        indexNode["depend"] = YAML::Node(YAML::NodeType::Sequence);
    }

    bool dependencyExists = false;
    for (const auto &dep : indexNode["depend"])
    {
        if (dep.as<std::string>() == packagesUrl)
        {
            dependencyExists = true;
            break;
        }
    }
    if (!dependencyExists)
    {
        indexNode["depend"].push_back(packagesUrl);
    }

    if (!indexNode["packages"])
    {
        indexNode["packages"] = YAML::Node(YAML::NodeType::Sequence);
    }

    std::ofstream indexFileOut(repoDir / "pkg-list.yaml");
    indexFileOut << indexNode;
    indexFileOut.close();
    return 0;
}
int openspm_repo::initializeRepository(const std::string &repoPath, const RepositoryInfo &repoInfo)
{
    std::string metadata = generateRepositoryMetadata(repoInfo);
    std::filesystem::path repoDir(repoPath);
    if (!std::filesystem::exists(repoDir))
    {
        std::cerr << "Error: Repository path does not exist." << std::endl;
        return 1;
    }
    std::ofstream metaFile(repoDir / "repository.yaml");
    metaFile << metadata;
    metaFile.close();
    std::ofstream indexFile(repoDir / "pkg-list.yaml");
    indexFile << "[]"; // Empty YAML array
    indexFile.close();
    return 0;
}