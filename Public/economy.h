#pragma once

#include <string>
#include <optional>
#include <SQLiteCpp/SQLiteCpp.h>

#ifdef Economy_EXPORTS
#  define ECOAPI __declspec(dllexport)
#else
#  define ECOAPI __declspec(dllimport)
#endif

class Player;

namespace Mod::Economy {

ECOAPI int64_t GetBalance(Player *);
ECOAPI std::optional<std::string> UpdateBalance(Player *, int64_t unit, std::string const &reason);

class Transaction {
  SQLite::Transaction trans;

public:
  ECOAPI Transaction();
  ECOAPI void Commit();
};

} // namespace Mod::Economy