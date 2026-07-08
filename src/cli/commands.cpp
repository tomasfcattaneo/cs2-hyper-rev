#include "commands.hpp"

#include "cs2/triggerbot.hpp"
#include "kernel/cr3_resolve.hpp"
#include "kernel/slat_cr3_capture.hpp"
#include "memory/attach_cache.hpp"
#include "memory/drv.hpp"
#include "memory/game_attach.hpp"
#include "memory/hyperrev_memory.hpp"

#include <iostream>
#include <string>

namespace cli::commands {

namespace {

std::wstring widen(const std::string_view s)
{
    return std::wstring(s.begin(), s.end());
}

[[nodiscard]] std::uint32_t resolve_pid(const options& opts)
{
    if (opts.pid != 0)
        return opts.pid;
    const auto found = hyperrev_memory::find_pid(opts.process);
    return found ? *found : 0;
}

[[nodiscard]] std::uint64_t resolve_base(const options& opts, const std::uint32_t pid)
{
    if (opts.base != 0)
        return opts.base;
    return hyperrev_memory::module_base(pid, widen(opts.process));
}

} // namespace

int capture(const options& opts)
{
    const std::uint32_t pid = resolve_pid(opts);
    if (pid == 0) {
        std::cerr << "[FAIL] proceso no encontrado: " << opts.process << '\n';
        return 70;
    }

    std::uint64_t base = resolve_base(opts, pid);
    if (base == 0) {
        cr3_resolve::options ropts{};
        ropts.refuse_if_apex_running = false;
        ropts.skip_mz_validate = true;
        if (const auto walk = cr3_resolve::resolve_target(opts.process, pid, ropts))
            base = walk->image_base;
    }
    if (base == 0) {
        std::cerr << "[FAIL] no se pudo resolver la base del módulo\n";
        return 71;
    }

    slat_cr3_capture::options sopts{};
    sopts.verbose = opts.verbose;
    sopts.dry_run = opts.dry_run;

    const auto session = slat_cr3_capture::capture_apex_cr3(opts.process, pid, base, sopts);
    if (!session) {
        if (opts.dry_run)
            return 0;
        std::cerr << "[FAIL] captura de SLAT CR3 falló\n";
        return 72;
    }

    if (!attach_cache::save(opts.process, *session))
        std::cerr << "[WARN] fallo al guardar el cache\n";

    std::cout << "[capture] pid=" << session->pid
              << " cr3=0x" << std::hex << session->cr3
              << " base=0x" << session->image_base << std::dec << '\n';
    return 0;
}

int verify(const options& opts)
{
    const auto session = game_attach::from_cache(opts.process);
    if (!session) {
        std::cerr << "[FAIL] no hay cache válido — ejecuta --capture primero\n";
        return 10;
    }

    std::cout << "[verify] pid=" << session->pid
              << " cr3=0x" << std::hex << session->cr3
              << " base=0x" << session->image_base << std::dec << '\n';
    std::cout << "[verify] MZ válido\n";
    return 0;
}

int trigger(const options& opts)
{
    ::cs2_triggerbot::options t{};
    t.verbose = opts.verbose;
    t.attach_cache_process = opts.process;
    t.trigger_distance = opts.trigger_distance;
    t.trigger_delay_ms = opts.trigger_delay_ms;
    return ::cs2_triggerbot::run(t);
}

} // namespace cli::commands
