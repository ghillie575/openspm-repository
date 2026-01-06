#include <openspm_repo_cli.hpp>
#include <openspm_repo.hpp>
#include <string>
#include <iostream>
#include <vector>
namespace openspm_repo
{
    int processCommandLine(std::string command,
                           const std::vector<std::string> &commandArgs,
                           const std::vector<std::pair<std::string, std::string>> &flagsWithValues,
                           const std::vector<std::string> &flagsWithoutValues)
    {
        if (command == "rebuild_index")
        {

            if (commandArgs.size() < 1)
            {
                std::cerr << "Usage: rebuild_index <repo_path>\n";
                return 1;
            }
            rebuildRepositoryIndex(commandArgs[0]);
        }
        else if (command == "init_repo")
        {
            if (commandArgs.size() < 1)
            {
                std::cerr << "Usage: init_repo <repo_path>\n";
                return 1;
            }
            RepositoryInfo repoInfo;
            for (const auto &flag : flagsWithValues)
            {
                if (flag.first == "--url")
                {
                    repoInfo.url = flag.second;
                }
                else if (flag.first == "--name")
                {
                    repoInfo.name = flag.second;
                }
                else if (flag.first == "--description")
                {
                    repoInfo.description = flag.second;
                }
                else if (flag.first == "--maintainer")
                {
                    repoInfo.mantainer = flag.second;
                }
            }
            initializeRepository(commandArgs[0], repoInfo);
        }
        else if (command == "add_url")
        {
            if (commandArgs.size() < 2)
            {
                std::cerr << "Usage: add_url <repo_path> <packages_url>\n";
                return 1;
            }
            addUrlToRepository(commandArgs[0], commandArgs[1]);
        }
        else
        {
            std::cout << "Unknown command: " << command << "\n";
            return 1;
        }
        return 0;
    }
} // namespace openspm_repo_cli