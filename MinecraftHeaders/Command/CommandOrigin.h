#pragma once

#include "../Core/BlockPos.h"
#include "../Core/Vec3.h"
#include "../Core/mce.h"
#include "CommandPermissionLevel.h"
#include <optional>

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
  __declspec(dllimport) virtual ~CommandOrigin();
  virtual std::string const &getRequestId() const            = 0;
  virtual std::string getName() const                        = 0;
  virtual BlockPos getBlockPosition() const                  = 0;
  virtual Vec3 getWorldPosition() const                      = 0;
  virtual Level *getLevel() const                            = 0;
  virtual Dimension *getDimension() const                    = 0;
  virtual Actor *getEntity() const                           = 0;
  virtual CommandPermissionLevel getPermissionsLevel() const = 0;
  virtual std::unique_ptr<CommandOrigin> clone() const       = 0;
  __declspec(dllimport) virtual std::optional<BlockPos> getCursorHitBlockPos() const;
  __declspec(dllimport) virtual std::optional<Vec3> getCursorHitPos() const;
  __declspec(dllimport) virtual bool hasChatPerms() const;
  __declspec(dllimport) virtual bool hasTellPerms() const;
  __declspec(dllimport) virtual bool canUseAbility(AbilitiesIndex) const;
  __declspec(dllimport) virtual bool isWorldBuilder() const;
  __declspec(dllimport) virtual bool canUseCommandsWithoutCheatsEnabled() const;
  __declspec(dllimport) virtual bool isSelectorExpansionAllowed() const;
  __declspec(dllimport) virtual NetworkIdentifier const &getSourceId() const;
  __declspec(dllimport) virtual unsigned chargetSourceSubId() const;
  __declspec(dllimport) virtual CommandOrigin const &getOutputReceiver() const;
  virtual CommandOriginType getOriginType() const = 0;
  __declspec(dllimport) virtual CommandOriginData toCommandOriginData() const;
  __declspec(dllimport) virtual mce::UUID const &getUUID() const;
  __declspec(dllimport) virtual void handleCommandOutputCallback(Json::Value &&) const;

protected:
  __declspec(dllimport) virtual void _setUUID(mce::UUID const &);
};