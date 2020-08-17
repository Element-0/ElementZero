#pragma once

#include <functional>

#include <Core/json.h>

#include <base/event.h>
#include <base/playerdb.h>

#ifdef FormUI_EXPORTS
#  define FORMAPI __declspec(dllexport)
#else
#  define FORMAPI __declspec(dllimport)
#endif

namespace Mod {

class FormUI : public EventEmitter<
                   "server_settings"_sig, PlayerEntry const &,
                   CallbackToken<std::pair<Json::Value, std::function<void(Json::Value const &)>>> &> {
  FORMAPI FormUI();

public:
  FORMAPI static FormUI &GetInstance();
  FORMAPI void
  SendModalForm(PlayerEntry const &target, Json::Value const &req, std::function<void(Json::Value const &)> resp);

  USING_EVENTEMITTER(
      "server_settings", PlayerEntry const &,
      CallbackToken<std::pair<Json::Value, std::function<void(Json::Value const &)>>> &);
};

} // namespace Mod