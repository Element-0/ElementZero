#include <string>
#include <type_traits>

#include <msgpack.hpp>

#include "serialization.h"

template <typename T, typename = void> struct auto_pack {
  auto_pack(msgpack::packer<msgpack::sbuffer> &pk, T const &val) { pk.pack(val); }
};

template <typename T> auto_pack(msgpack::packer<msgpack::sbuffer> &pk, T const &val)->auto_pack<T>;

template <typename T> void pack(msgpack::packer<msgpack::sbuffer> &pk, T const &val) { auto_pack(pk, val); }

template <typename T> struct auto_pack<T, std::enable_if_t<std::is_enum_v<T>>> {
  auto_pack(msgpack::packer<msgpack::sbuffer> &pk, T const &val) { pk.pack((std::underlying_type_t<T>) val); }
};

template <typename T> struct auto_pack<std::vector<T>> {
  auto_pack(msgpack::packer<msgpack::sbuffer> &pk, std::vector<T> const &val) {
    pk.pack_array(val.size());
    for (auto &item : val) pack(pk, item);
  }
};

template <typename K, typename V> struct auto_pack<std::map<K, V>> {
  auto_pack(msgpack::packer<msgpack::sbuffer> &pk, std::map<K, V> const &val) {
    pk.pack_map(val.size());
    for (auto &[k, v] : val) {
      pack(pk, k);
      pack(pk, v);
    }
  }
};

template <typename K, typename V> struct auto_pack<std::unordered_map<K, V>> {
  auto_pack(msgpack::packer<msgpack::sbuffer> &pk, std::unordered_map<K, V> const &val) {
    pk.pack_map(val.size());
    for (auto &[k, v] : val) {
      pack(pk, k);
      pack(pk, v);
    }
  }
};

#define DEF_PACKER(T) void pack(msgpack::packer<msgpack::sbuffer> &pk, T const &val)

DEF_PACKER(ItemStack) {
  pk.pack_map(3);
  pack(pk, "id");
  pack(pk, val.getId());
  pack(pk, "aux");
  pack(pk, val.getAuxValue());
  pack(pk, "name");
  pack(pk, val.getName());
}

DEF_PACKER(InventorySource) {
  pk.pack_map(3);
  pack(pk, "type");
  pack(pk, val.type);
  pack(pk, "container");
  pack(pk, val.container);
  pack(pk, "flags");
  pack(pk, val.flags);
}

DEF_PACKER(InventoryAction) {
  pk.pack_map(4);
  pack(pk, "source");
  pack(pk, val.source);
  pack(pk, "slot");
  pack(pk, val.slot);
  pack(pk, "from");
  pack(pk, val.from);
  pack(pk, "to");
  pack(pk, val.to);
}

DEF_PACKER(InventoryTransactionItemGroup) {
  pk.pack_map(4);
  pack(pk, "id");
  pack(pk, val.itemId);
  pack(pk, "aux");
  pack(pk, val.itemAux);
  pack(pk, "count");
  pack(pk, val.count);
  pack(pk, "flag");
  pack(pk, val.flag);
}

DEF_PACKER(InventoryTransaction) {
  pk.pack_map(2);
  pack(pk, "actions");
  pack(pk, val.actions);
  pack(pk, "items");
  pack(pk, val.items);
}

DEF_PACKER(NetworkBlockPosition) {
  pk.pack_map(3);
  pack(pk, "x");
  pack(pk, val.x);
  pack(pk, "y");
  pack(pk, val.y);
  pack(pk, "z");
  pack(pk, val.z);
}

DEF_PACKER(Vec3) {
  pk.pack_map(3);
  pack(pk, "x");
  pack(pk, val.x);
  pack(pk, "y");
  pack(pk, val.y);
  pack(pk, "z");
  pack(pk, val.z);
}

DEF_PACKER(ItemUseInventoryTransaction) {
  pk.pack_map(8);
  pack(pk, "type");
  pack(pk, val.type);
  pack(pk, "data");
  pack(pk, val.data);
  pack(pk, "actionType");
  pack(pk, val.actionType);
  pack(pk, "pos");
  pack(pk, val.pos);
  pack(pk, "face");
  pack(pk, val.face);
  pack(pk, "itemInHand");
  pack(pk, val.itemInHand);
  pack(pk, "playerPos");
  pack(pk, val.playerPos);
  pack(pk, "clickPos");
  pack(pk, val.clickPos);
}

DEF_PACKER(ItemUseOnActorInventoryTransaction) {
  pk.pack_map(7);
  pack(pk, "type");
  pack(pk, val.type);
  pack(pk, "data");
  pack(pk, val.data);
  pack(pk, "actionType");
  pack(pk, val.actionType);
  pack(pk, "slot");
  pack(pk, val.slot);
  pack(pk, "itemInHand");
  pack(pk, val.itemInHand);
  pack(pk, "playerPos");
  pack(pk, val.playerPos);
  pack(pk, "clickPos");
  pack(pk, val.clickPos);
}

DEF_PACKER(ItemReleaseInventoryTransaction) {
  pk.pack_map(6);
  pack(pk, "type");
  pack(pk, val.type);
  pack(pk, "data");
  pack(pk, val.data);
  pack(pk, "actionType");
  pack(pk, val.actionType);
  pack(pk, "slot");
  pack(pk, val.slot);
  pack(pk, "itemInHand");
  pack(pk, val.itemInHand);
  pack(pk, "playerPos");
  pack(pk, val.playerPos);
}

DEF_PACKER(ComplexInventoryTransaction) {
  using Type = ComplexInventoryTransaction::Type;
  switch (val.type) {
  case Type::NORMAL:
  case Type::MISMATCH: {
    pk.pack_map(2);
    pack(pk, "type");
    pack(pk, val.type);
    pack(pk, "data");
    pack(pk, val.data);
  } break;
  case Type::ITEM_USE: {
    pack(pk, (ItemUseInventoryTransaction const &) val);
  } break;
  case Type::ITEM_USE_ON_ACTOR: {
    pack(pk, (ItemUseOnActorInventoryTransaction const &) val);
  } break;
  case Type::RELEASE_ITEM: {
    pack(pk, (ItemReleaseInventoryTransaction const &) val);
  } break;
  default: pk.pack_nil();
  }
}

std::string SerializeInventoryTransaction(ComplexInventoryTransaction const &trans) {
  msgpack::sbuffer buffer;
  msgpack::packer pk{&buffer};
  pack(pk, trans);
  return {buffer.data(), buffer.size()};
}