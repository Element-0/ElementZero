#pragma once

#include <memory>
#include "../dll.h"

class CommandContext;
struct MCRESULT;

class MinecraftCommands {
public:
  MCAPI MCRESULT executeCommand(std::shared_ptr<CommandContext> ctx, bool print) const;
};