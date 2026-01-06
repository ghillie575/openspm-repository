#include <iostream>
#include <string>
#include <vector>
#include <openspm_repo_cli.hpp>
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <command> [args...] [flags...]\n";
        return 1;
    }
    std::string command = argv[1];

    // Parse arguments and flags
    std::vector<std::string> commandArgs;
    std::vector<std::pair<std::string, std::string>> flagsWithValues;
    std::vector<std::string> flagsWithoutValues;

    for (int i = 2; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg.rfind("-", 0) == 0)
        {
            // Long flags (--flag)
            if (arg.rfind("--", 0) == 0)
            {
                std::size_t eqPos = arg.find('=');
                if (eqPos != std::string::npos)
                {
                    // Flag with value using = syntax (--flag=value)
                    std::string flag = arg.substr(0, eqPos);
                    std::string value = arg.substr(eqPos + 1);
                    flagsWithValues.emplace_back(flag, value);
                }
                else if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    // Flag with value as next argument (--flag value)
                    flagsWithValues.emplace_back(arg, argv[++i]);
                }
                else
                {
                    // Boolean flag (--flag)
                    flagsWithoutValues.push_back(arg);
                }
            }
            else
            {
                // Short flags (-f)
                flagsWithoutValues.push_back(arg);
            }
        }
        else
        {
            // Positional argument
            commandArgs.push_back(arg);
        }
    }
    return openspm_repo::processCommandLine(command, commandArgs, flagsWithValues, flagsWithoutValues);
}
