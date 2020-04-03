#pragma once

#include <uuidsupport.h>
#include <flatbuffers/flatbuffers.h>

namespace Mod::proto {
struct UUID;
}

namespace flatbuffers {
Mod::proto::UUID Pack(mce::UUID const &obj);
mce::UUID UnPack(const Mod::proto::UUID &obj);
} // namespace flatbuffers
