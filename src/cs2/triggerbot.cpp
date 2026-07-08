#include "triggerbot.hpp"

#include "entity.hpp"
#include "offsets.hpp"
#include "memory/drv.hpp"
#include "memory/game_attach.hpp"
#include "hypercall/hypercall.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

namespace cs2_triggerbot {

namespace {

std::atomic<bool> g_stop{false};

void on_sigint(const int) { g_stop = true; }

// Simula mouse click a través de hipercall (Ring-1)
void trigger_shot()
{
    // TODO: Implementar simulación de click del mouse
    // Por ahora, solo logging
    std::cout << "[trigger] CLICK!\n";
}

} // namespace

int run(const options& opts)
{
    std::cout << "[triggerbot] process=" << opts.attach_cache_process << '\n';

    const auto session = game_attach::open(opts.attach_cache_process, opts.verbose);
    if (!session) {
        std::cerr << "[FAIL] attach failed — is CS2 running? try --capture first\n";
        return 80;
    }

    const std::uint64_t game_base = drv::GetModuleBase();
    std::cout << "[triggerbot] cr3=0x" << std::hex << session->cr3
              << " base=0x" << game_base << std::dec << '\n';
    std::cout << "[triggerbot] trigger_distance=" << opts.trigger_distance << '\n';

    std::signal(SIGINT, on_sigint);

    int ticks = 0;
    while (!g_stop && (opts.max_ticks == 0 || ticks < opts.max_ticks)) {
        // Obtener jugador local
        const auto local_controller = cs2_entity::get_local_player_controller(game_base);
        if (local_controller == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(opts.trigger_delay_ms));
            ++ticks;
            continue;
        }

        const auto local_pawn = cs2_entity::get_player_pawn_from_controller(local_controller);
        if (local_pawn == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(opts.trigger_delay_ms));
            ++ticks;
            continue;
        }

        const auto local_data = cs2_entity::get_player_data(local_pawn);
        if (!local_data.is_valid || local_data.health <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(opts.trigger_delay_ms));
            ++ticks;
            continue;
        }

        const auto local_eye_pos = cs2_entity::get_player_eye_position(local_pawn);

        // Iterar entidades
        bool triggered = false;
        for (int i = 0; i < cs2_offsets::max_players; ++i) {
            const auto entity_ptr = cs2_entity::get_entity_ptr(game_base + cs2_offsets::entity_list, i);
            if (entity_ptr == 0)
                continue;

            const auto entity_data = cs2_entity::get_player_data(entity_ptr);
            if (!entity_data.is_valid)
                continue;

            // Validaciones básicas
            if (entity_data.health <= 0)  // Muerto
                continue;
            if (entity_data.team == local_data.team)  // Es aliado
                continue;
            if (entity_data.life_state != 0)  // No está vivo
                continue;

            // Calcular distancia
            const float dist = cs2_entity::distance(local_eye_pos, entity_data.position);
            if (dist > opts.trigger_distance)  // Demasiado lejos
                continue;

            // TODO: Añadir raycast/visibility check aquí

            if (opts.verbose)
                std::cout << "[triggerbot] enemy found at distance " << dist << "m\n";

            trigger_shot();
            triggered = true;
            break;
        }

        if (opts.verbose && (ticks % 100 == 0))
            std::cout << "[triggerbot] tick " << ticks << '\n';

        std::this_thread::sleep_for(std::chrono::milliseconds(opts.trigger_delay_ms));
        ++ticks;
    }

    std::cout << "[triggerbot] stopped (" << ticks << " ticks)\n";
    return 0;
}

} // namespace cs2_triggerbot
