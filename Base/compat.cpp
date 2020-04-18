#include <base.h>
#include <Actor/Player.h>
#include <Core/Minecraft.h>
#include <Container/SimpleContainer.h>
#include <Command/CommandOutput.h>
#include <Level/Level.h>
#include <Item/Item.h>
#include <Net/NetworkIdentifier.h>
#include <Net/ServerNetworkHandler.h>
#include <RakNet/RakPeer.h>

#include "loader.h"

template <typename Holder> struct ValueHolder {
  Holder value;
  ~ValueHolder() {}
  operator Holder() const { return value; }
};

template <typename Ret, typename Type> Ret &direct_access(Type *type, size_t offset) {
  union {
    size_t raw;
    Type *source;
    Ret *target;
  } u;
  u.source = type;
  u.raw += offset;
  return *u.target;
}

#pragma region Player

// Actor::Actor
SimpleContainer &Actor::getEquipmentContainer() { return direct_access<SimpleContainer>(this, 1400); }
// Actor::Actor
SimpleContainer &Actor::getHandContainer() { return direct_access<SimpleContainer>(this, 1672); }
// Player::GetSelectedItem
PlayerInventoryProxy &Player::getInventory() { return *direct_access<PlayerInventoryProxy *>(this, 3528); }
// ServerPlayer::ServerPlayer
Certificate &Player::getCertificate() { return *direct_access<Certificate *>(this, 3248); }
// Player::setBedRespawnPosition
BlockPos const &Player::getSpawnPosition() const { return direct_access<BlockPos const>(this, 7740); }
// ServerPlayer::ServerPlayer
NetworkIdentifier const &Player::getNetworkIdentifier() const {
  return direct_access<NetworkIdentifier const>(this, 8536);
}
// AddPlayerPacket::AddPlayerPacket
std::string const &Player::getDeviceId() const { return direct_access<std::string>(this, 8360); }

#pragma endregion

void NetworkIdentifier::kick(std::string const &reason) const {
  LocateService<ServerNetworkHandler>()->disconnectClient(*this, reason, reason.empty());
}

void Player::kick() { LocateService<ServerNetworkHandler>()->forceDisconnectClient(this, true); }

std::string &ServerNetworkHandler::GetMotd() { return direct_access<std::string>(this, 608); }

void CommandOutput::success() { direct_access<bool>(this, 40) = true; }

uint64_t Level::GetServerTick() {
  return CallServerClassMethod<ValueHolder<uint64_t>>("?getCurrentServerTick@Level@@UEBA?BUTick@@XZ", this);
}

LevelDataWrapper &Level::GetLevelDataWrapper() { return direct_access<LevelDataWrapper>(this, 536); }

template <> Minecraft *LocateService<Minecraft>() {
  return *GetServerSymbol<Minecraft *>("?mGame@ServerCommand@@1PEAVMinecraft@@EA");
}

template <> ServerNetworkHandler *LocateService<ServerNetworkHandler>() {
  return LocateService<Minecraft>()->getServerNetworkHandler();
}

template <> MinecraftCommands *LocateService<MinecraftCommands>() { return LocateService<Minecraft>()->getCommands(); }

MinecraftCommands *Minecraft::getCommands() { return direct_access<MinecraftCommands *>(this, 160); }

bool Item::getAllowOffhand() const { return direct_access<char>(this, 258) & 0x40; }

RakNet::SystemAddress NetworkIdentifier::getRealAddress() const {
  return LocateService<RakNet::RakPeer>()->GetSystemAddressFromGuid(guid);
}

TClasslessInstanceHook(bool, "?loadLevelData@DBStorage@@UEAA_NAEAVLevelData@@@Z", void *data) {
  auto path = direct_access<std::string>(this, 144);
  worldHook(std::filesystem::weakly_canonical(path));
  return original(this, data);
}