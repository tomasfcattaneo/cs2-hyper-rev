#include "entity.hpp"
#include "offsets.hpp"
#include "memory/drv.hpp"

namespace cs2_entity {

std::uint64_t get_local_player_controller(const std::uint64_t game_base)
{
    return drv::Read<std::uint64_t>(game_base + cs2_offsets::local_player_controller);
}

std::uint64_t get_player_pawn_from_controller(const std::uint64_t controller)
{
    if (controller == 0)
        return 0;
    
    // En CS2, el pawn es un handle que necesita ser resuelto
    const auto handle = drv::Read<std::uint32_t>(controller + cs2_offsets::controller_pawn_handle);
    
    // El handle codifica: index (16 bits) + serial (16 bits)
    const auto index = handle & 0xFFFF;
    
    // Resolver desde entity list
    return get_entity_ptr(drv::GetModuleBase() + cs2_offsets::entity_list, index);
}

PlayerData get_player_data(const std::uint64_t pawn)
{
    PlayerData data{};
    data.pawn_ptr = pawn;
    
    if (pawn == 0) {
        data.is_valid = false;
        return data;
    }
    
    data.health = drv::Read<int>(pawn + cs2_offsets::pawn_health);
    data.team = drv::Read<int>(pawn + cs2_offsets::pawn_team_num);
    data.life_state = drv::Read<int>(pawn + cs2_offsets::pawn_life_state);
    data.position = get_player_position(pawn);
    data.is_valid = true;
    
    return data;
}

std::uint64_t get_entity_ptr(const std::uint64_t entity_list, const int index)
{
    if (index < 0 || index >= cs2_offsets::max_players)
        return 0;
    
    const auto entity_addr = entity_list + (static_cast<std::uint64_t>(index) << cs2_offsets::entity_list_stride_shift);
    return drv::Read<std::uint64_t>(entity_addr);
}

glm::vec3 get_player_position(const std::uint64_t pawn)
{
    if (pawn == 0)
        return glm::vec3(0);
    return drv::Read<glm::vec3>(pawn + cs2_offsets::pawn_origin);
}

glm::vec3 get_player_eye_position(const std::uint64_t pawn)
{
    if (pawn == 0)
        return glm::vec3(0);
    return drv::Read<glm::vec3>(pawn + cs2_offsets::pawn_eye_position);
}

} // namespace cs2_entity
