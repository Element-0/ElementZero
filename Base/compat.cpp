#include <Actor/Player.h>
#include <Core/Minecraft.h>
#include <Container/SimpleContainer.h>
#include <Command/CommandOutput.h>
#include <Level/Level.h>
#include <Item/Item.h>
#include <Net/NetworkIdentifier.h>
#include <Net/ServerNetworkHandler.h>
#include <RakNet/RakPeer.h>

#include <modutils.h>

#include <base/base.h>

#include "loader.h"

template <typename Holder> struct ValueHolder {
  Holder value;
  ~ValueHolder() {}
  operator Holder() const { return value; }
};

#pragma region Player

// CommandBlock::use
BlockSource *Actor::getRegion() const { return direct_access<BlockSource *>(this, 800); }
// Actor::_sendDirtyActorData
ActorRuntimeID Actor::getRuntimeID() const { return direct_access<ActorRuntimeID>(this, 1272); }
// AddPlayerPacket::AddPlayerPacket(Player &)
SynchedActorData &Actor::getEntityData() const { return direct_access<SynchedActorData>(this, 304); }
// Actor::Actor
SimpleContainer &Actor::getEquipmentContainer() const { return direct_access<SimpleContainer>(this, 1400); }
// Actor::Actor
SimpleContainer &Actor::getHandContainer() const { return direct_access<SimpleContainer>(this, 1672); }
// xref to Actor::transferTickingArea
class Dimension *Actor::getDimension() const {
  return direct_access<class Dimension *>(this, 808);
}
// Player::GetSelectedItem
PlayerInventoryProxy &Player::getInventory() const { return *direct_access<PlayerInventoryProxy *>(this, 3528); }
// ServerPlayer::ServerPlayer
Certificate &Player::getCertificate() const { return *direct_access<class Certificate *>(this, 3248); }
// Player::setBedRespawnPosition
BlockPos &Player::getSpawnPosition() const { return direct_access<BlockPos>(this, 7740); }
// ServerPlayer::ServerPlayer
NetworkIdentifier const &Player::getNetworkIdentifier() const {
  return direct_access<class NetworkIdentifier const>(this, 8536);
}
// AddPlayerPacket::AddPlayerPacket
std::string &Player::getDeviceId() const { return direct_access<std::string>(this, 8360); }
// ServerNetworkHandler::_createNewPlayer
std::string &Player::getClientPlatformId() const { return direct_access<std::string>(this, 3256); }
// ServerNetworkHandler::_createNewPlayer
std::string &Player::getPlatformOfflineId() const { return direct_access<std::string>(this, 3192); }
// ServerNetworkHandler::_createNewPlayer
std::string &Player::getClientPlatformOnlineId() const { return direct_access<std::string>(this, 4120); }
// RaidBossComponent::_sendBossEvent
unsigned char Player::getClientSubId() const { return direct_access<unsigned char>(this, 4112); }

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

ActorUniqueID Level::getNewUniqueID() const {
  auto &r = direct_access<uint64_t>(this, 408);
  return ++r;
}

// RaidBossComponent::_sendBossEvent
PacketSender &Level::getPacketSender() const { return *direct_access<PacketSender *>(this, 2096); }

LevelDataWrapper &Level::GetLevelDataWrapper() { return direct_access<LevelDataWrapper>(this, 536); }

template <> Minecraft *LocateService<Minecraft>() {
  return *GetServerSymbol<Minecraft *>("?mGame@ServerCommand@@1PEAVMinecraft@@EA");
}

template <> ServerNetworkHandler *LocateService<ServerNetworkHandler>() {
  return LocateService<Minecraft>()->getServerNetworkHandler();
}

template <> NetworkHandler *LocateService<NetworkHandler>() {
  return direct_access<std::unique_ptr<NetworkHandler>>(LocateService<ServerInstance>(), 40).get();
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