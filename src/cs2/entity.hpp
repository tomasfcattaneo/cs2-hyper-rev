#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace cs2_entity {

struct PlayerData {
    std::uint64_t controller_ptr;
    std::uint64_t pawn_ptr;
    int health;
    int team;
    glm::vec3 position;
    int life_state;
    bool is_valid;
};

// Lee jugador local
[[nodiscard]] std::uint64_t get_local_player_controller(std::uint64_t game_base);
[[nodiscard]] std::uint64_t get_player_pawn_from_controller(std::uint64_t controller);

// Lee datos del jugador desde pawn
[[nodiscard]] PlayerData get_player_data(std::uint64_t pawn);

// Itera lista de entidades
[[nodiscard]] std::uint64_t get_entity_ptr(std::uint64_t entity_list, int index);

// Position helpers
[[nodiscard]] glm::vec3 get_player_position(std::uint64_t pawn);
[[nodiscard]] glm::vec3 get_player_eye_position(std::uint64_t pawn);

// Distancia entre dos puntos
[[nodiscard]] inline float distance(const glm::vec3& a, const glm::vec3& b)
{
    return glm::distance(a, b);
}

} // namespace cs2_entity
