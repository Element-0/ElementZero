#pragma once
#include "base.h"
#include "event.h"
#include "uuidsupport.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <Actor/Player.h>

namespace Mod {

struct PlayerEntry {
  Player *player;
  std::string name;
  uint64_t xuid;
  mce::UUID uuid;
};

using PlayerEntryContainer = boost::multi_index_container<
    PlayerEntry,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<boost::multi_index::member<PlayerEntry, Player *, &PlayerEntry::player>>,
        boost::multi_index::ordered_unique<boost::multi_index::member<PlayerEntry, std::string, &PlayerEntry::name>>,
        boost::multi_index::ordered_unique<boost::multi_index::member<PlayerEntry, uint64_t, &PlayerEntry::xuid>>,
        boost::multi_index::hashed_unique<boost::multi_index::member<PlayerEntry, mce::UUID, &PlayerEntry::uuid>>>>;

class PlayerDatabase : public EventEmitter<"joined"_sig, PlayerEntry const &>,
                       public EventEmitter<"left"_sig, PlayerEntry const &> {
  PlayerEntryContainer data;
  BASEAPI PlayerDatabase();

public:
  inline PlayerEntryContainer const &GetData() const { return data; }
  BASEAPI static PlayerDatabase &GetInstance();

  USING_EVENTEMITTER("joined", PlayerEntry const &);
  USING_EVENTEMITTER("left", PlayerEntry const &);
};

} // namespace Mod