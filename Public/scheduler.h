#pragma once

#include <functional>
#include <chrono>
#include <exception>

#include "base.h"

namespace Mod::Scheduler {

using namespace std::chrono_literals;
using Token   = size_t;
using Handler = std::function<void(Token)>;
using Tick    = std::chrono::duration<uint64_t, std::ratio<1, 20>>;

constexpr Tick operator"" _tick(unsigned long long secs) { return Tick(secs); }

BASEAPI Token SetTimeOut(Tick timeout, Handler fn);
BASEAPI void ClearTimeOut(Token token);

BASEAPI Token SetInterval(Tick interval, Handler fn);
BASEAPI void ClearInterval(Token token);

} // namespace Mod::Scheduler