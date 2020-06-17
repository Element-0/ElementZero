#pragma once

#include <functional>
#include <chrono>
#include <exception>

#include "base.h"

namespace Mod::Scheduler {

using namespace std::chrono_literals;
using Token    = size_t;
using Handler  = std::function<void(Token)>;
using GameTick = std::chrono::duration<uint64_t, std::ratio<1, 20>>;

constexpr GameTick operator"" _tick(unsigned long long secs) { return GameTick(secs); }

BASEAPI Token SetTimeOut(GameTick timeout, Handler fn);
BASEAPI void ClearTimeOut(Token token);

BASEAPI Token SetInterval(GameTick interval, Handler fn);
BASEAPI void ClearInterval(Token token);

BASEAPI void RunInMainThread(std::function<void()> &&);
BASEAPI void RunInWorkerThread(std::function<void()> &&);

} // namespace Mod::Scheduler