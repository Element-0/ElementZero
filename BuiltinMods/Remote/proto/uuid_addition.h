#pragma once

#include <uuidsupport.h>
#include <flatbuffers/flatbuffers.h>

namespace Mod::proto {
struct UUID;
}

namespace flatbuffers {
Mod::proto::UUID Pack(std::unique_ptr<mce::UUID> const &obj);
std::unique_ptr<mce::UUID> UnPack(const Mod::proto::UUID &obj);
} // namespace flatbuffers
