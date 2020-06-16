#pragma once

#include <string>
#include <list>
#include <functional>

#ifdef Bus_EXPORTS
#  define BUSAPI __declspec(dllexport)
#else
#  define BUSAPI __declspec(dllimport)
#endif

namespace Mod::Bus {

enum class ACL {
  Public,
  Protected,
  Private,
};

/* Add method to bus (This function can be called during init) */
BUSAPI void PostInitAction(std::string const &name, void (*fn)());

/* Add method to bus */
BUSAPI void AddMethod(std::string const &key, std::function<std::string(std::string_view)> fn);

/* Broadcast event on key */
BUSAPI void Broadcast(std::string_view key, std::string_view data);

/* Call external service */
BUSAPI std::string Call(std::string_view bucket, std::string_view key, std::string_view data);

/* Set key acl */
BUSAPI void SetACL(std::string_view key, ACL acl);

/* List keys for external service */
BUSAPI std::list<std::string> ListKeys(std::string_view bucket);

/* Set kv for itself */
BUSAPI void SetKey(std::string_view key, std::string_view data);
/* Set kv for external service */
BUSAPI void SetKey(std::string_view bucket, std::string_view key, std::string_view data);

/* Fetch kv for itself */
BUSAPI std::string GetKey(std::string_view key);
/* Fetch kv for external service */
BUSAPI std::string GetKey(std::string_view bucket, std::string_view key);

/* Delete kv for itself */
BUSAPI void DelKey(std::string_view key);
/* Delete kv for external service */
BUSAPI void DelKey(std::string_view bucket, std::string_view key);

} // namespace Mod::Bus