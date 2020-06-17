#pragma once

#include <cstdint>
#include <optional>
#include <variant>

#include <base/event.h>
#include <base/playerdb.h>

#ifdef Blacklist_EXPORTS
#  define BLACKLISTAPI __declspec(dllexport)
#else
#  define BLACKLISTAPI __declspec(dllimport)
#endif

namespace Mod {

struct Blacklist {
  static BLACKLISTAPI Blacklist &GetInstance();

  struct XUID {
    std::int64_t value;
    std::optional<std::string> name;
    explicit XUID(std::uint64_t value, std::string name) : value((std::int64_t) value), name(name) {}
    explicit XUID(std::uint64_t value) : value((std::int64_t) value) {}
    std::int64_t const &operator*() const { return value; }
  };
  struct UUID {
    mce::UUID value;
    std::optional<std::string> name;
    explicit UUID(mce::UUID value, std::string name) : value(value), name(name) {}
    explicit UUID(mce::UUID value) : value(value) {}
    mce::UUID const &operator*() const { return value; }
  };
  struct NAME {
    std::string value;
    explicit NAME(std::string value) : value(value) {}
    std::string const &operator*() const { return value; }
  };

  using Entry = std::variant<XUID, UUID, NAME>;

  virtual void Add(Entry const &entry, std::string const &reason, std::string const &op) = 0;
  virtual bool Remove(Entry const &entry)                                                = 0;
};

} // namespace Mod