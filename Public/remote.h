#pragma once

#include <string_view>

#include <ws-gw.h>

#ifdef Remote_EXPORTS
#  define REMOTEAPI __declspec(dllexport)
#else
#  define REMOTEAPI __declspec(dllimport)
#endif

namespace Mod {

struct Remote {
  static REMOTEAPI Remote &GetInstance();

  virtual void AddMethod(std::string const &name, WsGw::Handler handler);
  virtual void AddMethod(std::string const &name, WsGw::SyncHandler handler);
  virtual void Broadcast(std::string_view name, WsGw::BufferView payload);
};

} // namespace Mod