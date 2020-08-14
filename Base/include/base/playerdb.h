#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <typeinfo>
#include <typeindex>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/tag.hpp>

#include <Core/AutomaticID.h>
#include <Actor/Player.h>
#include <Actor/Actor.h>
#include <Net/NetworkIdentifier.h>
#include <Level/ChangeDimensionRequest.h>

#include "base.h"
#include "event.h"
#include "uuidsupport.h"

namespace Mod {

struct AuxHolder {
  virtual ~AuxHolder(){};
};

// Player database entry structure
struct PlayerEntry {
  Player *player;
  std::string name;
  uint64_t xuid;
  mce::UUID uuid;
  NetworkIdentifier netid;
};

struct OfflinePlayerEntry {
  std::string name;
  uint64_t xuid;
  mce::UUID uuid;
};

using PlayerEntryContainer = boost::multi_index_container<
    PlayerEntry,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
            boost::multi_index::tag<Player *>, boost::multi_index::member<PlayerEntry, Player *, &PlayerEntry::player>>,
        boost::multi_index::ordered_unique<
            boost::multi_index::tag<std::string>,
            boost::multi_index::member<PlayerEntry, std::string, &PlayerEntry::name>>,
        boost::multi_index::ordered_unique<
            boost::multi_index::tag<uint64_t>, boost::multi_index::member<PlayerEntry, uint64_t, &PlayerEntry::xuid>>,
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<mce::UUID>, boost::multi_index::member<PlayerEntry, mce::UUID, &PlayerEntry::uuid>>,
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<NetworkIdentifier>,
            boost::multi_index::member<PlayerEntry, NetworkIdentifier, &PlayerEntry::netid>>>>;

// Player database for searching players or subscribing to player join and leave events
class PlayerDatabase
    : public EventEmitter<"joined"_sig, PlayerEntry const &>,
      public EventEmitter<"initialized"_sig, PlayerEntry const &>,
      public EventEmitter<"left"_sig, PlayerEntry const &>,
      public EventEmitter<"change_dimension"_sig, PlayerEntry const &, ChangeDimensionRequest const &, bool> {
  PlayerEntryContainer data;
  std::map<Player *, std::map<std::type_index, std::shared_ptr<AuxHolder>>> auxmap;
  BASEAPI PlayerDatabase();

public:
  inline void SetAux(Player *target, std::shared_ptr<AuxHolder> &&val) {
    if (!val) return;
    auto &rval                = *val;
    std::type_index const tid = typeid(rval);
    auxmap[target].emplace(tid, std::move(val));
  }

  template <typename T> std::shared_ptr<T> GetAux(Player *target) {
    std::type_index const tid = typeid(T);
    auto &aux                 = auxmap[target];
    if (auto it = aux.find(tid); it != aux.end()) return std::dynamic_pointer_cast<T>(it->second);
    return nullptr;
  }

  template <typename T> T &GetAuxAuto(Player *target) {
    std::type_index const tid = typeid(T);
    auto &aux                 = auxmap[target];
    if (auto it = aux.find(tid); it != aux.end())
      return *std::dynamic_pointer_cast<T>(it->second);
    else {
      auto ret = std::make_shared<T>();
      aux.emplace(tid, ret);
      return *ret;
    }
  }

  inline PlayerEntryContainer const &GetData() const { return data; }
  BASEAPI static PlayerDatabase &GetInstance();
  BASEAPI std::optional<PlayerEntry> Find(Player *player);
  BASEAPI std::optional<PlayerEntry> Find(std::string const &name);
  BASEAPI std::optional<PlayerEntry> Find(uint64_t xuid);
  BASEAPI std::optional<PlayerEntry> Find(mce::UUID const &uuid);
  BASEAPI std::optional<PlayerEntry> Find(NetworkIdentifier const &netid);
  BASEAPI std::optional<PlayerEntry> GeneralFind(std::string const &text);

  BASEAPI std::optional<OfflinePlayerEntry> FindOffline(std::string const &name);
  BASEAPI std::optional<OfflinePlayerEntry> FindOffline(uint64_t xuid);
  BASEAPI std::optional<OfflinePlayerEntry> FindOffline(mce::UUID const &uuid);
  BASEAPI std::optional<OfflinePlayerEntry> GeneralFindOffline(std::string const &text);

  USING_EVENTEMITTER("joined", PlayerEntry const &);
  USING_EVENTEMITTER("initialized", PlayerEntry const &);
  USING_EVENTEMITTER("left", PlayerEntry const &);
  USING_EVENTEMITTER("change_dimension", PlayerEntry const &, ChangeDimensionRequest const &, bool);
};

} // namespace Mod