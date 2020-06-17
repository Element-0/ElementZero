#include <boost/scope_exit.hpp>

#include <SQLiteCpp/SQLiteCpp.h>

#include <base/playerdb.h>
#include <mods/Economy.h>

#include "global.h"

int64_t Mod::Economy::GetBalance(Player *player) {
  auto &pdb  = Mod::PlayerDatabase::GetInstance();
  auto entry = pdb.Find(player);
  static SQLite::Statement stmt{*db, "SELECT value FROM balance WHERE uuid = ?"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt.clearBindings();
    stmt.reset();
  };
  stmt.bindNoCopy(1, entry->uuid, sizeof entry->uuid);
  return stmt.executeStep() ? stmt.getColumn(0).getInt64() : 0;
}

std::optional<std::string> Mod::Economy::UpdateBalance(Player *player, int64_t unit, std::string const &reason) {
  auto &pdb  = Mod::PlayerDatabase::GetInstance();
  auto entry = pdb.Find(player);
  static SQLite::Statement stmt{*db, "INSERT INTO transaction_log (uuid, value, reason) VALUES (?, ?, ?)"};
  BOOST_SCOPE_EXIT_ALL() {
    stmt.clearBindings();
    stmt.tryReset();
  };
  stmt.bindNoCopy(1, entry->uuid, sizeof entry->uuid);
  stmt.bind(2, unit);
  stmt.bindNoCopy(3, reason);
  try {
    stmt.exec();
    return {};
  } catch (SQLite::Exception const &ex) { return ex.what(); }
}

Mod::Economy::Transaction::Transaction() : trans(*db) {}
void Mod::Economy::Transaction::Commit() { trans.commit(); }
