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
  struct PLAYER {};
  struct NAME {};
  struct XUID {};
  Player *player;
  std::string name;
  std::string xuid;
  mce::UUID uuid;
};

using PlayerEntryContainer = boost::multi_index_container<
    PlayerEntry,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
            boost::multi_index::tag<PlayerEntry::PLAYER>,
            boost::multi_index::member<PlayerEntry, Player *, &PlayerEntry::player>>,
        boost::multi_index::ordered_unique<
            boost::multi_index::tag<PlayerEntry::NAME>,
            boost::multi_index::member<PlayerEntry, std::string, &PlayerEntry::name>>,
        boost::multi_index::ordered_unique<
            boost::multi_index::tag<PlayerEntry::XUID>,
            boost::multi_index::member<PlayerEntry, std::string, &PlayerEntry::xuid>>,
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<UUID>, boost::multi_index::member<PlayerEntry, mce::UUID, &PlayerEntry::uuid>>>>;

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