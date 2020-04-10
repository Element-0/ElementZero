#include <exception>
#include <stdexcept>

#include <flatbuffers/flatbuffers.h>

#include <Core/json.h>
#include <Core/MCRESULT.h>
#include <Command/MinecraftCommands.h>
#include <Command/CommandContext.h>
#include <Core/ServerInstance.h>

#include <remote.h>
#include <command.h>
#include <base.h>

#include "global.h"
#include "proto/command_generated.h"

static RegisterAPI reg("Command", false, [] {
  Mod::Remote::GetInstance().AddMethod("execute_command", [](WsGw::Buffer buffer, auto fn) {
    try {
      using namespace Mod::proto;
      using namespace Mod;
      flatbuffers::Verifier verifier{buffer.data(), buffer.size()};
      auto req = flatbuffers::GetRoot<CommandRequst>(buffer.data());
      if (!req->Verify(verifier)) throw std::runtime_error{"Failed to parse arguments"};
      LocateService<ServerInstance>()->queueForServerThread(
          [name = req->name()->str(), command = req->command()->str(), fn]() {
            auto origin      = std::make_unique<CustomCommandOrigin>();
            origin->name     = name;
            origin->callback = [fn](Json::Value &&value) {
              flatbuffers::FlatBufferBuilder builder;
              auto msg = value["statusMessage"].asString("");
              value.removeMember("statusMessage");
              Json::FastWriter writer;
              auto val = writer.write(value);
              builder.Finish(CreateCommandResponseDirect(builder, msg.c_str(), val.c_str()));
              fn(nullptr, builder);
            };
            auto ctx    = CommandContext::create(command, std::move(origin));
            auto result = LocateService<MinecraftCommands>()->executeCommand(std::move(ctx), false);
            if (result != MCRESULT_Success)
              fn(std::make_exception_ptr(std::runtime_error{"failed to execute command"}), {});
          });
    } catch (std::exception const &ex) { fn(std::make_exception_ptr(ex), {}); }
  });
});