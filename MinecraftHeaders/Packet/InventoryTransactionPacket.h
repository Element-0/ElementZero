#pragma once

#include "../Core/NBT.h"
#include "../Core/Packet.h"
#include "../Core/ContainerID.h"
#include "../Math/NetworkBlockPosition.h"
#include "../Math/BlockFace.h"
#include "../Actor/Player.h"
#include "../Actor/ActorRuntimeID.h"
#include "../Item/ItemStack.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

class ReadOnlyBinaryStream;
class BinaryStream;
class ItemStack;
class CompoundTag;

enum class InventoryTransactionError {
  Unexcepted = 7,
};

enum class InventorySourceType {
  Invalid                = -1,
  Global                 = 1,
  WorldInteraction       = 2,
  Creative               = 3,
  UntrackedInteractionUI = 100,
  NONIMPLEMENTEDTODO     = 99999
};

class InventorySource {
public:
  InventorySourceType type;
  ContainerID container = ContainerID::Invalid;
  enum class InventorySourceFlags { DropItem, PickupItem, None } flags;

  inline InventorySource(ContainerID id) : container(id) {}
  inline InventorySource(InventorySourceType type) : type(type) {}
  inline InventorySource(InventorySourceType type, InventorySourceFlags flags) : type(type), flags(flags) {}
  inline InventorySource(InventorySourceType type, ContainerID id) : type(type), container(id) {}

  constexpr inline bool operator==(InventorySource const &rhs) const noexcept {
    return type == rhs.type && container == rhs.container && flags == rhs.flags;
  }

  static inline InventorySource fromContainerWindowID(ContainerID id) { return {id}; }
  static inline InventorySource fromCreativeInventory() { return {InventorySourceType::Creative}; }
  static inline InventorySource fromGlobalInventory() { return {InventorySourceType::Global}; }
  static inline InventorySource fromWorldInteraction(InventorySourceFlags flags) {
    return {InventorySourceType::WorldInteraction, flags};
  }
  static inline InventorySource fromInvalid() { return {InventorySourceType::Invalid}; }
  static inline InventorySource fromUntrackedInteractionUI(ContainerID id) {
    return {InventorySourceType::UntrackedInteractionUI, id};
  }
  static inline InventorySource fromNONIMPLEMENTEDTODO(ContainerID id) {
    return {InventorySourceType::NONIMPLEMENTEDTODO, id};
  }
};

namespace std {
template <> struct hash<InventorySource> {
  constexpr uint64_t operator()(InventorySource const &source) const noexcept {
    return (uint64_t) source.type << 16 ^ (uint64_t) source.container;
  }
};
} // namespace std

class InventoryAction {
public:
  InventorySource source;
  unsigned int slot;
  ItemStack from, to;
};

class InventoryTransactionItemGroup {
public:
  unsigned short itemId;
  unsigned short itemAux;
  std::unique_ptr<CompoundTag> tag;
  int count;
  unsigned char flag;

  __declspec(dllimport) InventoryTransactionItemGroup(ItemStack const &, int);
  inline ~InventoryTransactionItemGroup() {}
};

class InventoryTransaction {
public:
  std::unordered_map<InventorySource, std::vector<InventoryAction>> actions;
  std::vector<InventoryTransactionItemGroup> items;

  __declspec(dllimport) void addAction(InventoryAction const &);
  __declspec(dllimport) InventoryTransactionError executeFull(Player &, bool) const;
  __declspec(dllimport) void forceBalanceTransaction();
  __declspec(dllimport) std::vector<InventoryAction> const &getActions(InventorySource const &) const;
  __declspec(dllimport)
      std::function<InventoryTransactionError(Player &, InventoryAction const &, bool)> getVerifyFunction(
          InventorySource const &) const;
  __declspec(dllimport) InventoryTransactionError verifyFull(Player &, bool) const;

private:
  __declspec(dllimport) void addActionToContent(InventoryAction const &);
  __declspec(dllimport) void _dropCreatedItems(Player &);
  __declspec(dllimport) void addItemToContent(ItemStack const &);

public:
  inline InventoryTransaction() {}
  inline InventoryTransaction(std::vector<InventoryAction> actions) {
    for (auto &act : actions) addAction(act);
  }
};

static_assert(offsetof(InventoryTransaction, items) == 64);

class ComplexInventoryTransaction {
public:
  enum class Type : unsigned {
    NORMAL            = 0,
    MISMATCH          = 1,
    ITEM_USE          = 2,
    ITEM_USE_ON_ACTOR = 3,
    RELEASE_ITEM      = 4,
  } type;
  InventoryTransaction data;

  inline virtual ~ComplexInventoryTransaction() {}
  __declspec(dllimport) virtual void read(ReadOnlyBinaryStream &);
  __declspec(dllimport) virtual void write(BinaryStream &) const;
  __declspec(dllimport) virtual InventoryTransactionError handle(Player &, bool) const;
  __declspec(dllimport) virtual void onTransactionError(Player &, InventoryTransactionError) const;
};

static_assert(sizeof(ComplexInventoryTransaction) == 104);

class ItemUseInventoryTransaction : public ComplexInventoryTransaction {
public:
  int actionType;
  NetworkBlockPosition pos;
  uint32_t unk;
  BlockFace face;
  uint32_t slot;
  ItemStack itemInHand;
  Vec3 playerPos, clickPos;

  inline virtual ~ItemUseInventoryTransaction() {}
  __declspec(dllimport) virtual void read(ReadOnlyBinaryStream &);
  __declspec(dllimport) virtual void write(BinaryStream &) const;
  __declspec(dllimport) virtual InventoryTransactionError handle(Player &, bool) const;
  __declspec(dllimport) virtual void onTransactionError(Player &, InventoryTransactionError) const;
};

class ItemUseOnActorInventoryTransaction : public ComplexInventoryTransaction {
public:
  ActorRuntimeID actorId;
  int actionType;
  uint32_t slot;
  ItemStack itemInHand;
  Vec3 playerPos, clickPos;

  inline virtual ~ItemUseOnActorInventoryTransaction() {}
  __declspec(dllimport) virtual void read(ReadOnlyBinaryStream &);
  __declspec(dllimport) virtual void write(BinaryStream &) const;
  __declspec(dllimport) virtual InventoryTransactionError handle(Player &, bool) const;
  __declspec(dllimport) virtual void onTransactionError(Player &, InventoryTransactionError) const;
};

class ItemReleaseInventoryTransaction : public ComplexInventoryTransaction {
public:
  int actionType;
  uint32_t slot;
  ItemStack itemInHand;
  Vec3 playerPos;

  inline virtual ~ItemReleaseInventoryTransaction() {}
  __declspec(dllimport) virtual void read(ReadOnlyBinaryStream &);
  __declspec(dllimport) virtual void write(BinaryStream &) const;
  __declspec(dllimport) virtual InventoryTransactionError handle(Player &, bool) const;
  __declspec(dllimport) virtual void onTransactionError(Player &, InventoryTransactionError) const;
};

class InventoryTransactionPacket : public Packet {
public:
  std::unique_ptr<ComplexInventoryTransaction> transaction;

  inline ~InventoryTransactionPacket() {}
  __declspec(dllimport) virtual MinecraftPacketIds getId() const;
  __declspec(dllimport) virtual std::string getName() const;
  __declspec(dllimport) virtual void write(BinaryStream &) const;
  __declspec(dllimport) virtual PacketReadResult read(ReadOnlyBinaryStream &);
};