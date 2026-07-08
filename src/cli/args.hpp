#pragma once

#include <cstdint>
#include <string>

namespace cli {

enum class command {
    help,
    capture,
    verify,
    trigger,
};

struct options {
    command verb{command::help};
    std::string process{"cs2.exe"};
    std::uint32_t pid{};
    std::uint64_t cr3{};
    std::uint64_t base{};
    bool verbose{};
    bool dry_run{};
    float trigger_distance{500.0f};
    int trigger_delay_ms{10};
};

options parse(int argc, char* argv[]);
void print_usage();

} // namespace cli
