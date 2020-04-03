#include <cstdint>
#include <optional>
#include <string>
#include <boost/scope_exit.hpp>
#include <boost/format.hpp>

#include <blacklist.h>
#include <playerdb.h>
#include <variant>

#include "Core/mce.h"
#include "settings.h"

template <typename T> std::string fetchName(T entry) {
  if (entry.name) { return *entry.name; }
  auto &db = Mod::PlayerDatabase::GetInstance();
  auto opt = db.FindOffline(*entry);
  if (opt)
    return opt->name;
  else
    return "Unknown name";
}

struct AddOp {
  std::string reason;
  std::string op;

  AddOp(std::string reason, std::string op) : reason(reason), op(op) {}

  void operator()(Mod::Blacklist::XUID const &xuid) {
    static SQLite::Statement insert_uuid = {*database,
                                            "INSERT OR REPLACE INTO xuid ("
                                            "value, name, reason, source) VALUES ("
                                            "?, ?, ?, ?)"};
    BOOST_SCOPE_EXIT_ALL() {
      insert_uuid.reset();
      insert_uuid.clearBindings();
    };
    auto source = (boost::format("BANNED BY %s") % op).str();
    insert_uuid.bind(1, *xuid);
    insert_uuid.bindNoCopy(2, fetchName(xuid));
    insert_uuid.bindNoCopy(3, reason);
    insert_uuid.bindNoCopy(4, source);
    insert_uuid.exec();
  }

  void operator()(Mod::Blacklist::UUID const &uuid) {
    static SQLite::Statement insert_uuid = {*database,
                                            "INSERT OR REPLACE INTO uuid ("
                                            "value, name, reason, source) VALUES ("
                                            "?, ?, ?, ?)"};
    BOOST_SCOPE_EXIT_ALL() {
      insert_uuid.reset();
      insert_uuid.clearBindings();
    };
    auto source = (boost::format("BANNED BY %s") % op).str();
    insert_uuid.bindNoCopy(1, *uuid, sizeof(mce::UUID));
    insert_uuid.bindNoCopy(2, fetchName(uuid));
    insert_uuid.bindNoCopy(3, reason);
    insert_uuid.bindNoCopy(4, source);
    insert_uuid.exec();
  }

  void operator()(Mod::Blacklist::NAME const &name) {
    static SQLite::Statement insert_uuid = {*database,
                                            "INSERT OR REPLACE INTO name ("
                                            "value, reason, source) VALUES ("
                                            "?, ?, ?)"};
    BOOST_SCOPE_EXIT_ALL() {
      insert_uuid.reset();
      insert_uuid.clearBindings();
    };
    auto source = (boost::format("BANNED BY %s") % op).str();
    insert_uuid.bindNoCopy(1, *name);
    insert_uuid.bindNoCopy(2, reason);
    insert_uuid.bindNoCopy(3, source);
    insert_uuid.exec();
  }
};

struct RemoveOp {
  bool operator()(Mod::Blacklist::XUID const &xuid) {
    static SQLite::Statement remove_xuid{*database, "DELETE FROM xuid WHERE value=?"};
    remove_xuid.bind(1, *xuid);
    BOOST_SCOPE_EXIT_ALL() {
      remove_xuid.reset();
      remove_xuid.clearBindings();
    };
    return remove_xuid.exec();
  }
  bool operator()(Mod::Blacklist::UUID const &uuid) {
    static SQLite::Statement remove_uuid{*database, "DELETE FROM uuid WHERE value=?"};
    remove_uuid.bindNoCopy(1, *uuid, sizeof *uuid);
    BOOST_SCOPE_EXIT_ALL() {
      remove_uuid.reset();
      remove_uuid.clearBindings();
    };
    return remove_uuid.exec();
  }
  bool operator()(Mod::Blacklist::NAME const &name) {
    static SQLite::Statement remove_name{*database, "DELETE FROM name WHERE value=?"};
    remove_name.bind(1, *name);
    BOOST_SCOPE_EXIT_ALL() {
      remove_name.reset();
      remove_name.clearBindings();
    };
    return remove_name.exec();
  }
};

namespace Mod {

struct BlacklistImpl : Blacklist {
  void Add(const Entry &entry, const std::string &reason, const std::string &op) override {
    std::visit(AddOp{reason, op}, entry);
  }
  bool Remove(const Entry &entry) override { return std::visit(RemoveOp{}, entry); }
};

Blacklist &Blacklist::GetInstance() {
  static BlacklistImpl impl;
  return impl;
}

} // namespace Mod