#pragma once

#include <optional>

#include <Core/mce.h>
#include <Math/Vec3.h>

#include "event.h"

#ifdef Essentials_EXPORTS
#  define ESSAPI __declspec(dllexport)
#else
#  define ESSAPI __declspec(dllimport)
#endif

class Player;

namespace Mod::Essentials {

struct WarpInfo {
  std::string Name;
  bool IsSystem;
  int Dimension;
  Vec3 Position;
};

class WarpSystem
    : public Mod::RecursiveEventEmitter<"warp"_sig, Player *, WarpInfo const &, Mod::CallbackToken<std::string> &>,
      public Mod::RecursiveEventEmitter<
          "set_warp"_sig, mce::UUID const &, WarpInfo const &, Mod::CallbackToken<std::string> &> {
  WarpSystem();

public:
  static WarpSystem &GetInstance();

  ESSAPI int GetGlobalWarpCount();
  ESSAPI std::vector<WarpInfo> GetGlobalWarpList();
  ESSAPI std::optional<WarpInfo> GetGlobalWarp(std::string const &name);
  ESSAPI void SetGlobalWarp(WarpInfo info);
  ESSAPI void DelGlobalWarp(std::string const &name);

  ESSAPI int GetWarpCount(mce::UUID const &uuid);
  ESSAPI std::vector<WarpInfo> GetWarpList(mce::UUID const &uuid);
  ESSAPI std::optional<WarpInfo> GetWarp(mce::UUID const &uuid, std::string const &name);
  ESSAPI std::optional<std::string> SetWarp(mce::UUID const &uuid, WarpInfo info);
  ESSAPI void DelWarp(mce::UUID const &uuid, std::string const &name);

  ESSAPI std::optional<std::string> Warp(Player *player, WarpInfo const &info);

  USING_RECEVENTEMITTER("warp", Player *, WarpInfo const &, Mod::CallbackToken<std::string> &);
  USING_RECEVENTEMITTER("set_warp", mce::UUID const &, WarpInfo const &, Mod::CallbackToken<std::string> &);
};

} // namespace Mod::Essentials