#include <Core/ServerInstance.h>

#include <base/base.h>
#include <mods/CommandSupport.h>

#include "global.h"

static RegisterAPI reg("CommandSupport", true, [] {
  client->register_handler("execute_command", [](std::string_view sv, auto resp) {
    using namespace mini_bus;
    using namespace Mod;
    LocateService<ServerInstance>()->queueForServerThread([=, command = std::string{sv}] {
      auto origin = std::make_unique<CustomCommandOrigin>();
      auto value  = CommandSupport::GetInstance().ExecuteCommand(std::move(origin), command);
      resp(value["statusMessage"].asString(""));
    });
  });

  client->register_handler("execute_command.json", [](std::string_view sv, auto resp) {
    using namespace mini_bus;
    using namespace Mod;
    LocateService<ServerInstance>()->queueForServerThread([=, command = std::string{sv}] {
      auto origin = std::make_unique<CustomCommandOrigin>();
      auto value  = CommandSupport::GetInstance().ExecuteCommand(std::move(origin), command);
      Json::FastWriter writer;
      resp(writer.write(value));
    });
  });
});