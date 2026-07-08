#pragma once

#include <cstdint>

namespace cs2_offsets {

// Entity System (dwEntityList desde sezzyaep/CS2-OFFSETS)
// NOTA: Estos offsets cambian con cada patch de CS2
// Actualizar desde: https://github.com/sezzyaep/CS2-OFFSETS
constexpr std::uint64_t entity_list        = 0x24B3268;
constexpr std::uint64_t entity_system_info = 0x23B47B8;
constexpr std::uint64_t highest_entity_index = 0x23B47B0;

// Local Player Controller
constexpr std::uint64_t local_player_controller = 0x26ABFF8;

// C_CSPlayerController offsets
constexpr std::uint64_t controller_pawn_handle = 0x958;

// C_CSPlayerPawn offsets
constexpr std::uint64_t pawn_health           = 0x334;   // m_iHealth
constexpr std::uint64_t pawn_team_num         = 0x3CB;   // m_iTeamNum
constexpr std::uint64_t pawn_life_state       = 0x352;   // m_lifeState
constexpr std::uint64_t pawn_origin           = 0xF48;   // m_vOldOrigin
constexpr std::uint64_t pawn_view_punch_angle = 0x1270;  // m_viewPunchAngle
constexpr std::uint64_t pawn_eye_position     = 0x12B8;  // m_vecEyePosition

// Entity stride
constexpr std::uint64_t entity_list_stride_shift = 5;

// Max players typical
constexpr int max_players = 64;

} // namespace cs2_offsets
