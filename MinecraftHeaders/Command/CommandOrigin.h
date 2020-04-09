#pragma once

#include <optional>
#include <memory>

#include "../Math/BlockPos.h"
#include "../Math/Vec3.h"
#include "../Core/mce.h"
#include "CommandPermissionLevel.h"
#include "../dll.h"

enum class CommandOriginType : char {
  Player           = 0,
  Block            = 1,
  MinecartBlock    = 2,
  DevConsole       = 3,
  Test             = 4,
  AutomationPlayer = 5,
  ClientAutomation = 6,
  DedicatedServer  = 7,
  Actor            = 8,
  Virtual          = 9,
  GameArgument     = 10,
  ActorServer      = 11
};

class CommandOriginData;
class Level;
class Dimension;
class Actor;
enum class AbilitiesIndex;
class NetworkIdentifier;
namespace Json {
class Value;
}

class CommandOrigin {
public:
  MCAPI virtual ~CommandOrigin();
  virtual std::string const &getRequestId() const            = 0;
  virtual std::string getName() const                        = 0;
  virtual BlockPos getBlockPosition() const                  = 0;
  virtual Vec3 getWorldPosition() const                      = 0;
  virtual Level *getLevel() const                            = 0;
  virtual Dimension *getDimension() const                    = 0;
  virtual Actor *getEntity() const                           = 0;
  virtual CommandPermissionLevel getPermissionsLevel() const = 0;
  virtual std::unique_ptr<CommandOrigin> clone() const       = 0;
  MCAPI virtual std::optional<BlockPos> getCursorHitBlockPos() const;
  MCAPI virtual std::optional<Vec3> getCursorHitPos() const;
  MCAPI virtual bool hasChatPerms() const;
  MCAPI virtual bool hasTellPerms() const;
  MCAPI virtual bool canUseAbility(AbilitiesIndex) const;
  MCAPI virtual bool isWorldBuilder() const;
  MCAPI virtual bool canUseCommandsWithoutCheatsEnabled() const;
  MCAPI virtual bool isSelectorExpansionAllowed() const;
  MCAPI virtual NetworkIdentifier const &getSourceId() const;
  MCAPI virtual unsigned chargetSourceSubId() const;
  MCAPI virtual CommandOrigin const &getOutputReceiver() const;
  virtual CommandOriginType getOriginType() const = 0;
  MCAPI virtual CommandOriginData toCommandOriginData() const;
  MCAPI virtual mce::UUID const &getUUID() const;
  MCAPI virtual void handleCommandOutputCallback(Json::Value &&) const;

protected:
  MCAPI virtual void _setUUID(mce::UUID const &);
};