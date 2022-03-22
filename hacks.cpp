#include "hacks.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace hacks {
Handler gHandler;
DataFile gDataFile;

bool DataFile::read(const std::string& fileName)
{
    std::ifstream openFile(fileName);
    if (!openFile.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(openFile, line)) {
        DataEntry entry;

        // First line is the name
        entry.m_name = line;

        // Second line is the library
        std::getline(openFile, line);
        entry.m_libraryName = line;

        // Third line is the address
        std::getline(openFile, line);
        entry.m_addr = std::strtol(line.c_str(), NULL, 0x10);

        // Fourth line is the "on" bytes
        {
            std::getline(openFile, line);
            std::istringstream iss { line };
            while (iss) {
                std::string token;
                iss >> token;

                if (!token.empty()) {
                    entry.m_onData.push_back(
                        static_cast<u8>(
                            std::strtol(token.c_str(), NULL, 0x10)));
                }
            }
        }

        // Fifth line is the "off" bytes
        {
            std::getline(openFile, line);
            std::istringstream iss { line };
            while (iss) {
                std::string token;
                iss >> token;

                if (!token.empty()) {
                    entry.m_offData.push_back(
                        static_cast<u8>(
                            std::strtol(token.c_str(), NULL, 0x10)));
                }
            }
        }

        m_entries.push_back(entry);
    }

    return true;
}

void tryHook()
{
    if (!gHandler.AttemptAttach("Geometry Dash", "GeometryDash.exe")) {
        std::cout << "Failed to attach to the GD client!" << std::endl;
        return;
    }

    std::cout << "Successfully hooked onto the GD client!" << std::endl;
}

void applyBytes(hacks::DataEntry& entry)
{
    std::vector<u8> data = entry.m_applyMode == ApplyMode::ApplyOn ? entry.m_onData : entry.m_offData;
    u32 addr = gHandler.GetModuleBase(entry.m_libraryName.data()) + entry.m_addr;
    u32 old = gHandler.Protect(addr, data.size(), PAGE_EXECUTE_READWRITE);

    if (!gHandler.Write(addr, data.data(), data.size())) {
        std::cout << "Unable to write hack to GD client!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    gHandler.Protect(addr, data.size(), old);

    if (entry.m_applyMode == ApplyMode::ApplyOn) {
        std::cout << "Toggling " << entry.m_name << " on..." << std::endl;
        entry.m_applyMode = ApplyMode::ApplyOff;
    } else {
        std::cout << "Toggling " << entry.m_name << " hack off..." << std::endl;
        entry.m_applyMode = ApplyMode::ApplyOn;
    }
}
}
