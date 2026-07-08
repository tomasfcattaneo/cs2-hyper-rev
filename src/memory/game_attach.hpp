#pragma once

#include "memory/attach_cache.hpp"
#include "memory/drv.hpp"
#include "memory/hyperrev_memory.hpp"
#include "kernel/slat_cr3_capture.hpp"

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace game_attach {

[[nodiscard]] inline bool verify_mz(const std::uint64_t base)
{
    const auto mz = drv::TryRead<std::uint16_t>(base);
    return mz && *mz == 0x5A4D;
}

[[nodiscard]] inline std::optional<hyperrev_memory::session> from_cache(
    const std::string_view process)
{
    const auto pid = hyperrev_memory::find_pid(process);
    if (!pid)
        return std::nullopt;
    const auto cached = attach_cache::load(process, *pid);
    if (!cached || !drv::SetSession(*cached) || !verify_mz(cached->image_base))
        return std::nullopt;
    return cached;
}

[[nodiscard]] inline std::optional<hyperrev_memory::session> capture_fresh(
    const std::string_view process,
    const bool verbose)
{
    const auto pid = hyperrev_memory::find_pid(process);
    if (!pid)
        return std::nullopt;

    const std::wstring wproc(process.begin(), process.end());
    const std::uint64_t base = hyperrev_memory::module_base(*pid, wproc);
    if (base == 0)
        return std::nullopt;

    slat_cr3_capture::options opts{};
    opts.verbose = verbose;
    const auto sess = slat_cr3_capture::capture_apex_cr3(process, *pid, base, opts);
    if (!sess)
        return std::nullopt;

    (void)attach_cache::save(process, *sess);
    if (!drv::SetSession(*sess))
        return std::nullopt;
    return sess;
}

// Cache hit first; inline SLAT capture on miss/stale.
[[nodiscard]] inline std::optional<hyperrev_memory::session> open(
    const std::string_view process,
    const bool verbose)
{
    if (auto cached = from_cache(process))
        return cached;

    if (verbose)
        std::cout << "[attach] cache miss/stale — SLAT capture...\n";
    return capture_fresh(process, verbose);
}

} // namespace game_attach
