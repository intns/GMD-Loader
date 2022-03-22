#include <handler.h>
#include <string>
#include <types.hpp>
#include <vector>

namespace hacks {
enum class ApplyMode : u8 {
    ApplyOn,
    ApplyOff
};

struct DataEntry {
    std::string m_name;
    std::string m_libraryName = "GeometryDash.exe";
    u32 m_addr = 0;

    std::vector<u8> m_onData {};
    std::vector<u8> m_offData {};

    ApplyMode m_applyMode = ApplyMode::ApplyOn;
};

struct DataFile {
    DataFile() = default;
    ~DataFile() = default;

    bool read(const std::string& fileName);

    const std::vector<DataEntry>& getEntries() const { return m_entries; }
    const std::vector<DataEntry>& getEntries() { return m_entries; }

private:
    std::vector<DataEntry> m_entries;
};

extern Handler gHandler;
extern DataFile gDataFile;

void tryHook();
void applyBytes(DataEntry& entry);
}