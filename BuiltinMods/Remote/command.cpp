#include <exception>
#include <remote.h>

#include <flatbuffers/flatbuffers.h>

#include <Core/json.h>
#include <Core/MCRESULT.h>
#include <Command/MinecraftCommands.h>
#include <Command/CommandContext.h>

#include <command.h>
#include <base.h>
#include <stdexcept>

#include "global.h"
#include "proto/command_generated.h"

static RegisterAPI reg("Command", false, [] {
  Mod::Remote::GetInstance().AddMethod("executeCommand", [](WsGw::Buffer const &buffer, auto fn) {
    try {
      using namespace Mod::proto;
      using namespace Mod;
      flatbuffers::Verifier verifier{buffer.data(), buffer.size()};
      auto req         = flatbuffers::GetRoot<CommandRequst>(buffer.data());
      auto origin      = std::make_unique<CustomCommandOrigin>();
      origin->name     = req->name()->str();
      origin->callback = [fn](Json::Value &&value) {
        flatbuffers::FlatBufferBuilder builder;
        auto msg = value["statusMessage"].asString("");
        value.removeMember("statusMessage");
        Json::FastWriter writer;
        auto val = writer.write(value);
        builder.Finish(CreateCommandResponseDirect(builder, msg.c_str(), val.c_str()));
        fn(nullptr, builder);
      };
      auto ctx    = CommandContext::create(req->command()->str(), std::move(origin));
      auto result = LocateService<MinecraftCommands>()->executeCommand(std::move(ctx), false);
      if (result != MCRESULT_Success) throw std::runtime_error{"failed to execute command"};
    } catch (...) { fn(std::current_exception(), {}); }
  });
});