#pragma once

#include <cstdint>
#include <string_view>

namespace cs2_triggerbot {

struct options {
    bool verbose{false};
    float trigger_distance{500.0f};
    int trigger_delay_ms{10};
    int max_ticks{0};  // 0 = run until Ctrl+C
    std::string_view attach_cache_process{"cs2.exe"};
};

int run(const options& opts);

} // namespace cs2_triggerbot
