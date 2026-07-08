#pragma once
// Minimal spdlog shim for cs2-trigger: silent no-ops.
// hyper-reV usermode source references spdlog/fmt heavily via LOG_INFO/LOG_ERR/LOG_WARN.
// We keep those log lines out of cs2-trigger's stdout by dropping them here.

#include <string>
#include <string_view>

namespace spdlog {

template <typename... Args>
inline void info(Args&&...) {}

template <typename... Args>
inline void warn(Args&&...) {}

template <typename... Args>
inline void error(Args&&...) {}

template <typename... Args>
inline void debug(Args&&...) {}

template <typename... Args>
inline void trace(Args&&...) {}

inline void set_pattern(std::string_view) {}
inline void set_pattern(const char*) {}

} // namespace spdlog

namespace fmt {

template <typename... Args>
inline void print(Args&&...) {}

} // namespace fmt
