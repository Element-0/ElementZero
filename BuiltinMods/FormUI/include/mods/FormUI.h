#pragma once

#include <functional>

#include <Core/json.h>

#include <base/playerdb.h>

#ifdef FormUI_EXPORTS
#  define FORMAPI __declspec(dllexport)
#else
#  define FORMAPI __declspec(dllimport)
#endif

namespace Mod::FormUI {

FORMAPI void SendModalForm(PlayerEntry const &target, Json::Value const &req, std::function<void(Json::Value const &)> resp);

} // namespace Mods::FormUI