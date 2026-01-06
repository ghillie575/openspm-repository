#pragma once
#include <string>
#include <vector>
#include <utility>
namespace openspm_repo
{
int processCommandLine(std::string command,
                       const std::vector<std::string> &commandArgs,
                       const std::vector<std::pair<std::string, std::string>> &flagsWithValues,
                       const std::vector<std::string> &flagsWithoutValues);
} // namespace openspm_repo_cli