#include "args.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace cli {

namespace {

std::uint64_t parse_u64(const std::string_view text)
{
    const int base = text.starts_with("0x") || text.starts_with("0X") ? 16 : 0;
    return std::stoull(std::string(text), nullptr, base);
}

} // namespace

options parse(const int argc, char* argv[])
{
    options opts{};
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i];

        if      (arg == "--capture")  opts.verb = command::capture;
        else if (arg == "--verify")   opts.verb = command::verify;
        else if (arg == "--trigger")  opts.verb = command::trigger;
        else if (arg == "--help" || arg == "-h")  opts.verb = command::help;

        else if (arg == "--dry-run")  opts.dry_run = true;
        else if (arg == "--verbose" || arg == "-v")  opts.verbose = true;

        else if ((arg == "--process" || arg == "-p") && i + 1 < argc)
            opts.process = argv[++i];
        else if (arg.starts_with("--process="))
            opts.process = std::string(arg.substr(10));
        else if ((arg == "--pid") && i + 1 < argc)
            opts.pid = static_cast<std::uint32_t>(parse_u64(argv[++i]));
        else if (arg.starts_with("--pid="))
            opts.pid = static_cast<std::uint32_t>(parse_u64(arg.substr(6)));
        else if ((arg == "--cr3") && i + 1 < argc)
            opts.cr3 = parse_u64(argv[++i]);
        else if (arg.starts_with("--cr3="))
            opts.cr3 = parse_u64(arg.substr(6));
        else if ((arg == "--base") && i + 1 < argc)
            opts.base = parse_u64(argv[++i]);
        else if (arg.starts_with("--base="))
            opts.base = parse_u64(arg.substr(7));
        else if ((arg == "--trigger-distance") && i + 1 < argc)
            opts.trigger_distance = static_cast<float>(std::stod(argv[++i]));
        else if (arg.starts_with("--trigger-distance="))
            opts.trigger_distance = static_cast<float>(std::stod(std::string(arg.substr(19))));
        else if ((arg == "--trigger-delay") && i + 1 < argc)
            opts.trigger_delay_ms = static_cast<int>(parse_u64(argv[++i]));
        else if (arg.starts_with("--trigger-delay="))
            opts.trigger_delay_ms = static_cast<int>(parse_u64(arg.substr(16)));
    }
    return opts;
}

void print_usage()
{
    std::cout <<
R"(cs2-trigger — Triggerbot para CS2 (hyper-reV Ring-1)

Usage: cs2-trigger [command] [options]

Commands:
  --capture              Capturar SLAT CR3 + guardar en cache
  --verify               Verificar que el cache sea válido
  --trigger              Ejecutar triggerbot (Ring-1)
  --help                 Este mensaje

Options:
  -p, --process NAME        Proceso objetivo (default: cs2.exe)
      --pid N               PID del proceso (para --capture)
      --base 0x...          Base del módulo (para --capture)
      --trigger-distance N  Distancia máxima para disparar (default: 500.0)
      --trigger-delay N     Delay entre disparos en ms (default: 10)
      --dry-run             Solo init, sin hook (--capture)
  -v, --verbose             Más logging

Ejemplos:
  cs2-trigger.exe --capture --process cs2.exe
  cs2-trigger.exe --verify
  cs2-trigger.exe --trigger --trigger-distance 300 --trigger-delay 5 --verbose
)";
}

} // namespace cli
