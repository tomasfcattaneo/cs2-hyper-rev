// cs2-loader — bootstrap para cargar hyper-reV
//
// Boot sequence:
//   [1/2] Verificar hyper-reV (hyperv-attachment cargado, read_guest_cr3 retorna CR3 válido)
//   [2/2] Lanzar cs2-trigger.exe
//
// Si algo falla, imprime [FAIL] y retorna con código de error.

#include <Windows.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#include "hypercall/hypercall.h"
#include "memory/hyperrev_memory.hpp"

namespace {

const char* k_cs2_process = "cs2.exe";
const char* k_trigger_exe = "cs2-trigger.exe";

const char* k_red   = "\x1b[91m";
const char* k_green = "\x1b[92m";
const char* k_yellow = "\x1b[93m";
const char* k_dim   = "\x1b[90m";
const char* k_reset = "\x1b[0m";

void enable_vt_processing()
{
    const HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (h == INVALID_HANDLE_VALUE || !GetConsoleMode(h, &mode))
        return;
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

[[nodiscard]] std::filesystem::path loader_dir()
{
    wchar_t buf[MAX_PATH]{};
    GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return std::filesystem::path(buf).parent_path();
}

[[nodiscard]] std::filesystem::path trigger_exe_path()
{
    return loader_dir() / k_trigger_exe;
}

void step(const int i, const int total, const std::string& what)
{
    std::cout << k_dim << "[" << i << "/" << total << "] " << k_reset << what << '\n';
}

void ok(const std::string& what)
{
    std::cout << "      " << k_green << "[OK]" << k_reset << " " << what << '\n';
}

void fail(const std::string& what)
{
    std::cout << "      " << k_red << "[FAIL]" << k_reset << " " << what << '\n';
}

[[nodiscard]] DWORD run_trigger(const std::string& args)
{
    const auto exe = trigger_exe_path();
    if (!std::filesystem::exists(exe)) {
        fail("cs2-trigger.exe not found next to loader (" + exe.string() + ")");
        return 0xFFFFFFFF;
    }

    std::string command = "\"" + exe.string() + "\" " + args;

    STARTUPINFOA si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};

    if (!CreateProcessA(nullptr, command.data(), nullptr, nullptr, FALSE,
                        0, nullptr, nullptr, &si, &pi)) {
        fail("CreateProcess failed err=" + std::to_string(GetLastError()));
        return 0xFFFFFFFF;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exit_code = 0;
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return exit_code;
}

} // namespace

int main()
{
    std::cout.setf(std::ios::unitbuf);
    std::cerr.setf(std::ios::unitbuf);

    enable_vt_processing();

    std::error_code ec;
    std::filesystem::current_path(loader_dir(), ec);

    std::cout << k_green
              << "cs2-loader — hyper-reV bootstrap\n"
              << k_reset
              << k_dim
              << "  workdir: " << std::filesystem::current_path().string() << '\n'
              << k_reset << '\n';

    // ---- Step 1: verify hyper-reV -----------------------------------------
    step(1, 2, "Verifying hyper-reV (hyperv-attachment)");
    const std::uint64_t caller_cr3 = hypercall::read_guest_cr3();
    if (caller_cr3 <= 0x1000) {
        fail("read_guest_cr3 returned 0x" + std::to_string(caller_cr3)
             + " — hyperv-attachment not loaded");
        return 1;
    }
    ok("caller_cr3 = 0x" + [caller_cr3] {
        char buf[24]{};
        std::snprintf(buf, sizeof(buf), "%llx", static_cast<unsigned long long>(caller_cr3));
        return std::string(buf);
    }());

    // ---- Step 2: launch cs2-trigger -----------------------------------------------
    step(2, 2, "Launching cs2-trigger");
    const DWORD trigger_rc = run_trigger("--capture --process cs2.exe");

    if (trigger_rc == 0) {
        std::cout << '\n' << k_green
                  << "cs2-loader: cs2-trigger exited cleanly\n"
                  << k_reset;
        return 0;
    }
    std::cout << '\n' << k_yellow
              << "cs2-loader: cs2-trigger exited with code "
              << trigger_rc << '\n' << k_reset;
    return static_cast<int>(trigger_rc);
}
