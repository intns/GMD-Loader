#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <util/misc.hpp>
#include <util/tokeniser.hpp>

#include <hacks.hpp>
#include <handler.h>

namespace {
struct Command {
    std::string m_command = "";
    std::string m_description = "";
    std::vector<std::string_view> m_parameters;
    std::function<void()> m_function;
};

static std::vector<Command> gCommands = {};

void showCommands()
{
    std::cout << std::endl
              << "Commands:" << std::endl;
    for (const Command& cmd : gCommands) {
        if (cmd.m_command == "NEW_LINE") {
            std::cout << std::endl;
            continue;
        }

        std::cout << " " << cmd.m_command << " ";

        for (std::size_t i = 0; i < cmd.m_parameters.size(); i++) {
            std::cout << "[" << cmd.m_parameters[i] << "] ";
        }

        std::cout << "- " << cmd.m_description << std::endl;
    }
    std::cout << std::endl;
}

void fillCommands()
{
    hacks::gDataFile.read("data-file.txt");
    gCommands.push_back({ "help", "re-generate this command list", {}, showCommands });
    gCommands.push_back({ "exit", "gracefully exits the program", {}, std::bind(&std::exit, EXIT_SUCCESS) });
    gCommands.push_back({ "NEW_LINE" });
    gCommands.push_back({ "hook", "try and hook to the GD client", {}, hacks::tryHook });
    gCommands.push_back({ "NEW_LINE" });

    const std::vector<hacks::DataEntry>& entries = hacks::gDataFile.getEntries();
    for (u32 i = 0; i < entries.size(); i++) {
        const hacks::DataEntry& entry = entries[i];

        gCommands.push_back({ std::to_string(i + 1), "apply the " + entry.m_name + " hack", {}, std::bind(&hacks::applyBytes, entry) });
    }
}
} // namespace

int main(int argc, char** argv)
{
    std::atexit(util::ExitHook);
    std::cout << "GMD-Loader by Axiot, 2022\n"
              << std::endl;

    // Try to hook to GD before we even create the commands and show them
    hacks::tryHook();

    fillCommands();
    showCommands();

    util::tokeniser parser;
    while (true) {
        std::string input = "";
        std::getline(std::cin, input);

        if (!input.size()) {
            break;
        }

        parser.read(input);
        const std::string& token = parser.next();

        bool foundCmd = false;
        for (const Command& cmd : gCommands) {
            if (cmd.m_command == "NEW_LINE") {
                continue;
            }

            if (cmd.m_command == token) {
                cmd.m_function();
                foundCmd = true;
                break;
            }
        }

        if (!foundCmd) {
            std::cout << "Unknown command " << token << std::endl;
        }

        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}