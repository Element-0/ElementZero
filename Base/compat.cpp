#include <base.h>
#include <Actor/Player.h>
#include <Core/Minecraft.h>
#include <Command/CommandOutput.h>
#include <Level/Level.h>
#include <Item/Item.h>
#include <Item/ItemStack.h>
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

Certificate &Player::getCertificate() { return *direct_access<Certificate *>(this, 3208); }

BlockPos const &Player::getSpawnPosition() const { return direct_access<BlockPos const>(this, 7548); }

NetworkIdentifier const &Player::getNetworkIdentifier() const {
  return direct_access<NetworkIdentifier const>(this, 2904);
}

void NetworkIdentifier::kick(std::string const &reason) const {
  LocateService<ServerNetworkHandler>()->disconnectClient(*this, reason, reason.empty());
}

void Player::kick() { LocateService<ServerNetworkHandler>()->forceDisconnectClient(this, true); }

void CommandOutput::success() { direct_access<bool>(this, 40) = true; }

uint64_t Level::GetServerTick() {
  return CallServerClassMethod<ValueHolder<uint64_t>>("?getCurrentServerTick@Level@@UEBA?BUTick@@XZ", this);
}

template <> Minecraft *LocateService<Minecraft>() {
  return *GetServerSymbol<Minecraft *>("?mGame@ServerCommand@@1PEAVMinecraft@@EA");
}

template <> ServerNetworkHandler *LocateService<ServerNetworkHandler>() {
  return LocateService<Minecraft>()->getServerNetworkHandler();
}

bool Item::getAllowOffhand() const { return direct_access<char>(this, 258) & 0x40; }

unsigned char ItemStackBase::getStackSize() const { return direct_access<unsigned char>(this, 34); }

RakNet::SystemAddress NetworkIdentifier::getRealAddress() const {
  return LocateService<RakNet::RakPeer>()->GetSystemAddressFromGuid(guid);
}

TClasslessInstanceHook(bool, "?loadLevelData@DBStorage@@UEAA_NAEAVLevelData@@@Z", void *data) {
  auto path = direct_access<std::string>(this, 144);
  worldHook(std::filesystem::weakly_canonical(path));
  return original(this, data);
}