#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "NBT.h"
#include "../Math/BlockPos.h"
#include "../Math/Vec3.h"
#include "../dll.h"

enum class DataItemType : unsigned char { BYTE, SHORT, INT, FLOAT, STRING, NBT, POS, LONG, VEC3 };

namespace DataTypeMap {

template <typename T> DataItemType typeFor;

template <> inline DataItemType typeFor<int8_t>      = DataItemType::BYTE;
template <> inline DataItemType typeFor<int16_t>     = DataItemType::SHORT;
template <> inline DataItemType typeFor<int32_t>     = DataItemType::INT;
template <> inline DataItemType typeFor<float>       = DataItemType::FLOAT;
template <> inline DataItemType typeFor<std::string> = DataItemType::STRING;
template <> inline DataItemType typeFor<CompoundTag> = DataItemType::NBT;
template <> inline DataItemType typeFor<BlockPos>    = DataItemType::POS;
template <> inline DataItemType typeFor<int64_t>     = DataItemType::LONG;
template <> inline DataItemType typeFor<Vec3>        = DataItemType::VEC3;

} // namespace DataTypeMap

class DataItem {
public:
  enum class Id : uint16_t {
    FLAGS                               = 0,  // LONG
    HEALTH                              = 1,  // INT (minecart/boat)
    VARIANT                             = 2,  // INT
    COLOR                               = 3,  // BYTE
    NAMETAG                             = 4,  // STRING
    OWNER                               = 5,  // LONG
    TARGET                              = 6,  // LONG
    AIR                                 = 7,  // SHORT
    POTION_COLOR                        = 8,  // INT (ARGB!)
    POTION_AMBIENT                      = 9,  // BYTE
    JUMP_DURATION                       = 10, // LONG
    HURT_TIME                           = 11, // INT (minecart/boat)
    HURT_DIRECTION                      = 12, // INT (minecart/boat)
    PADDLE_TIME_LEFT                    = 13, // FLOAT
    PADDLE_TIME_RIGHT                   = 14, // FLOAT
    EXPERIENCE_VALUE                    = 15, // INT (xp orb)
    DISPLAY_ITEM                        = 16, // INT (id | (data << 16))
    DISPLAY_OFFSET                      = 17, // INT
    HAS_DISPLAY                         = 18, // BYTE
    SWELL                               = 19,
    OLD_SWELL                           = 20,
    SWELL_DIR                           = 21,
    CHARGE_AMOUNT                       = 22,
    ENDERMAN_HELD_RUNTIME_ID            = 23, // SHORT
    ACTOR_AGE                           = 24, // SHORT
    PLAYER_FLAGS                        = 26, // BYTE
    PLAYER_INDEX                        = 27,
    PLAYER_BED_POSITION                 = 28, // POS
    FIREBALL_POWER_X                    = 29, // FLOAT
    FIREBALL_POWER_Y                    = 30, // FLOAT
    FIREBALL_POWER_Z                    = 31, // FLOAT
    AUX_POWER                           = 32,
    FISH_X                              = 33,
    FISH_Z                              = 34,
    FISH_ANGLE                          = 35,
    POTION_AUX_VALUE                    = 36, // SHORT
    LEAD_HOLDER                         = 37, // LONG
    SCALE                               = 38, // FLOAT
    INTERACTIVE_TAG                     = 39, // STRING
    NPC_SKIN_ID                         = 40, // STRING
    URL_TAG                             = 41, // STRING
    MAX_AIR                             = 42, // SHORT
    MARK_VARIANT                        = 43, // INT
    CONTAINER_TYPE                      = 44, // BYTE
    CONTAINER_BASE_SIZE                 = 45, // INT
    CONTAINER_EXTRA_SLOTS_PER_STRENGTH  = 46, // INT
    BLOCK_TARGET                        = 47, // POS (ENDER CRYSTAL)
    WITHER_INVULNERABLE_TICKS           = 48, // INT
    WITHER_TARGET_1                     = 49, // LONG
    WITHER_TARGET_2                     = 50, // LONG
    WITHER_TARGET_3                     = 51, // LONG
    AERIAL_ATTACK                       = 52,
    BOUNDING_BOX_WIDTH                  = 53, // FLOAT
    BOUNDING_BOX_HEIGHT                 = 54, // FLOAT
    FUSE_LENGTH                         = 55, // INT
    RIDER_SEAT_POSITION                 = 56, // VEC3
    RIDER_ROTATION_LOCKED               = 57, // BYTE
    RIDER_MAX_ROTATION                  = 58, // FLOAT
    RIDER_MIN_ROTATION                  = 59, // FLOAT
    AREA_EFFECT_CLOUD_RADIUS            = 60, // FLOAT
    AREA_EFFECT_CLOUD_WAITING           = 61, // INT
    AREA_EFFECT_CLOUD_PARTICLE_ID       = 62, // INT
    SHULKER_PEEK_ID                     = 63, // INT
    SHULKER_ATTACH_FACE                 = 64, // BYTE
    SHULKER_ATTACHED                    = 65, // SHORT
    SHULKER_ATTACH_POS                  = 66, // POS
    TRADING_PLAYER_EID                  = 67, // LONG
    TRADING_CAREER                      = 68,
    HAS_COMMAND_BLOCK                   = 69,
    COMMAND_BLOCK_COMMAND               = 70, // STRING
    COMMAND_BLOCK_LAST_OUTPUT           = 71, // STRING
    COMMAND_BLOCK_TRACK_OUTPUT          = 72, // BYTE
    CONTROLLING_RIDER_SEAT_NUMBER       = 73, // BYTE
    STRENGTH                            = 74, // INT
    MAX_STRENGTH                        = 75, // INT
    SPELL_CASTING_COLOR                 = 76, // INT
    LIMITED_LIFE                        = 77,
    ARMOR_STAND_POSE_INDEX              = 78, // INT
    ENDER_CRYSTAL_TIME_OFFSET           = 79, // INT
    ALWAYS_SHOW_NAMETAG                 = 80, // BYTE
    COLOR_2                             = 81, // BYTE
    NAME_AUTHOR                         = 82,
    SCORE_TAG                           = 83, // STRING
    BALLOON_ATTACHED_ENTITY             = 84, // LONG
    PUFFERFISH_SIZE                     = 85,
    BUBBLE_TIME                         = 86,
    AGENT                               = 87,
    SITTING_AMOUNT                      = 88,
    SITTING_AMOUNT_PREVIOUS             = 89,
    EATING_COUNTER                      = 90,
    FLAGS_EXTENDED                      = 91,
    LAYING_AMOUNT                       = 92,
    LAYING_AMOUNT_PREVIOUS              = 93,
    DURATION                            = 94,
    SPAWN_TIME                          = 95,
    CHANGE_RATE                         = 96,
    CHANGE_ON_PICKUP                    = 97,
    PICKUP_COUNT                        = 98,
    INTERACT_TEXT                       = 99,
    TRADE_TIER                          = 100,
    MAX_TRADE_TIER                      = 101,
    TRADE_EXPERIENCE                    = 102,
    SKIN_ID                             = 103, // INT
    SPAWNING_FRAMES                     = 104,
    COMMAND_BLOCK_TICK_DELAY            = 105,
    COMMAND_BLOCK_EXECUTE_ON_FIRST_TICK = 106,
    AMBIENT_SOUND_INTERVAL              = 107,
    AMBIENT_SOUND_EVENT_NAME            = 108,
    FALL_DAMAGE_MULTIPLIER              = 109,
    NAME_RAW_TEXT                       = 110,
    CAN_RIDE_TARGET                     = 111,
  };

  DataItemType type;
  Id id;
  bool dirty;

  DataItem(DataItemType type, Id id, bool dirty) : type(type), id(id), dirty(dirty) {}

  virtual ~DataItem() {}
  virtual MCAPI bool isDataEqual(DataItem const &rhs) const;
  virtual std::unique_ptr<DataItem> clone() const = 0;
};

template <typename T> class DataItem2 : public DataItem {
public:
  T value;
  DataItem2(Id id, T const &value) : DataItem(DataTypeMap::typeFor<T>, id, true), value(value) {}
  MCAPI std::unique_ptr<DataItem> clone() const override;
};