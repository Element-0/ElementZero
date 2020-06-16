#include <Core/ServerInstance.h>

#include <base.h>
#include <command.h>

#include "global.h"

static RegisterAPI reg("CommandSupport", true, [] {
  client->register_handler("execute_command", [](std::string_view sv) {
    using namespace mini_bus;
    using namespace Mod;
    NotifyToken<std::string> token;
    LocateService<ServerInstance>()->queueForServerThread([command = std::string{sv}, &token] {
      auto origin = std::make_unique<CustomCommandOrigin>();
      auto value  = CommandSupport::GetInstance().ExecuteCommand(std::move(origin), command);
      token.notify(value["statusMessage"].asString(""));
    });
    return token.wait();
  });

  client->register_handler("execute_command.json", [](std::string_view sv) {
    using namespace mini_bus;
    using namespace Mod;
    NotifyToken<std::string> token;
    LocateService<ServerInstance>()->queueForServerThread([command = std::string{sv}, &token] {
      auto origin = std::make_unique<CustomCommandOrigin>();
      auto value  = CommandSupport::GetInstance().ExecuteCommand(std::move(origin), command);
      Json::FastWriter writer;
      token.notify(writer.write(value));
    });
    return token.wait();
  });
});