#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include "boost/scope_exit.hpp"
#include "modutils.h"
#include "pch.h"
#include <hook.h>
#include "hook_init.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <map>

funchook *funchook_instance = funchook_create();

static std::map<void *, void **> hookmap;

int HookFunction(void *oldfunc, void **poutold, void *newfunc) {
  auto source = oldfunc;
  auto it     = hookmap.find(oldfunc);
  if (it != hookmap.end()) {
    *poutold    = *it->second;
    *it->second = newfunc;
    return 0;
  }
  auto code = funchook_prepare(funchook_instance, &oldfunc, newfunc);
  if (code == 0) {
    *poutold = oldfunc;
    hookmap.emplace(source, poutold);
  }
  return code;
}

void *GetServerSymbol(char const *name) {
  static auto handle = GetModuleHandle(nullptr);
  static SQLite::Database db{"addition_symbols.db"};
  static SQLite::Statement stmt{db, "SELECT rva FROM symbols WHERE symbol=?"};
  if (auto ret = (void *) GetProcAddress(handle, name); ret) return ret;
  BOOST_SCOPE_EXIT_ALL() {
    stmt.tryReset();
    stmt.clearBindings();
  };
  try {
    stmt.bindNoCopy(1, name);
    if (stmt.executeStep()) {
      printf("@@@@ %lld\n", stmt.getColumn(0).getInt64());
      return (void *) ((uint64_t) stmt.getColumn(0).getInt64() + (uint64_t) handle);
    }
    return 0;
  } catch (...) { return 0; }
}

FailedToHook::FailedToHook(int code) : exception(funchook_error_message(funchook_instance)) {}