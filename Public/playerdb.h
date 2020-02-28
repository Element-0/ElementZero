#pragma once
#include "base.h"
#include "event.h"
#include "uuidsupport.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/tag.hpp>
#include <Actor/Player.h>
#include <Net/NetworkIdentifier.h>

namespace Mod {

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
class PlayerDatabase : public EventEmitter<"joined"_sig, PlayerEntry const &>,
                       public EventEmitter<"left"_sig, PlayerEntry const &> {
  PlayerEntryContainer data;
  BASEAPI PlayerDatabase();

public:
  inline PlayerEntryContainer const &GetData() const { return data; }
  BASEAPI static PlayerDatabase &GetInstance();
  BASEAPI std::optional<PlayerEntry> Find(Player *player);
  BASEAPI std::optional<PlayerEntry> Find(std::string const &name);
  BASEAPI std::optional<PlayerEntry> Find(uint64_t xuid);
  BASEAPI std::optional<PlayerEntry> Find(mce::UUID const &uuid);
  BASEAPI std::optional<PlayerEntry> Find(NetworkIdentifier const &netid);

  BASEAPI std::optional<OfflinePlayerEntry> FindOffline(std::string const &name);
  BASEAPI std::optional<OfflinePlayerEntry> FindOffline(uint64_t xuid);
  BASEAPI std::optional<OfflinePlayerEntry> FindOffline(mce::UUID const &uuid);

  USING_EVENTEMITTER("joined", PlayerEntry const &);
  USING_EVENTEMITTER("left", PlayerEntry const &);
};

} // namespace Mod