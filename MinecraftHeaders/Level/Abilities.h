#pragma once

#include <memory>
#include <array>

#include "../Command/CommandPermissionLevel.h"
#include "PlayerPermissions.h"

class PermissionsHandler {
public:
  CommandPermissionLevel mCommandPermissionsLevel;
  PlayerPermissions mPlayerPermissions;
};

class Ability {
public:
  enum class Type : char { Invalid = 0, Boolean = 1, Float = 2 };
  enum class Options : char { Unset = 0, Saved = 1, Command = 2, Permission = 4 };
  union Value {
    bool val_bool;
    float val_float;
    Value(bool flag) { val_bool = flag; }
    Value(float val) { val_float = val; }
    Value() {}
  };
  Type type = Type::Invalid;
  Value value{};
  Options opts = Options::Unset;
};

class Abilities {
public:
  std::unique_ptr<PermissionsHandler> mPermissionsHandler;
  std::array<Ability, 18> Abilities;
  std::array<Ability, 8> CustomAbilities;
};