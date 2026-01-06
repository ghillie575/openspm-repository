#pragma once
#include <string>
#include <vector>
#include <openspm.hpp>
namespace openspm_repo
{
    int rebuildRepositoryIndex(const std::string &repoPath);
    int addUrlToRepository(const std::string &repoPath, const std::string &packagesUrl);
    int initializeRepository(const std::string &repoPath, const RepositoryInfo &repoInfo);
    int readPackageFromArchive(const std::string &archivePath, PackageInfo &pkgInfo);

} // namespace openspm_repo