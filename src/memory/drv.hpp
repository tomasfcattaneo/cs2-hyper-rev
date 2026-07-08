#pragma once

// hyper-reV-backed Read/Write shim (apex-simple style surface).

#include "memory/hyperrev_memory.hpp"
#include "hypercall/hypercall.h"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace drv {

inline hyperrev_memory::session g_session{};

inline bool SetSession(const hyperrev_memory::session& session)
{
    if (session.cr3 <= 0x1000 || session.image_base == 0)
        return false;
    g_session = session;
    return true;
}

inline bool HasSession()
{
    return g_session.cr3 > 0x1000 && g_session.image_base != 0;
}

template <typename T>
[[nodiscard]] std::optional<T> TryRead(const std::uint64_t address)
{
    if (!HasSession())
        return std::nullopt;
    return hyperrev_memory::read<T>(g_session, address);
}

template <typename T>
[[nodiscard]] T Read(const std::uint64_t address)
{
    return TryRead<T>(address).value_or(T{});
}

inline std::uint64_t GetModuleBase()
{
    return g_session.image_base;
}

template <typename T>
bool Write(const std::uint64_t address, const T& value)
{
    if (!HasSession())
        return false;
    return hypercall::write_guest_virtual_memory(
        &value, address, g_session.cr3, sizeof(T)) == sizeof(T);
}

} // namespace drv
