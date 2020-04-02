#include "uuid_generated.h"
#include <memory>
#include <type_traits>
#include <vcruntime_string.h>

namespace flatbuffers {
Mod::proto::UUID Pack(std::unique_ptr<mce::UUID> const &obj) {
  Mod::proto::UUID ret;
  memcpy(&ret, obj.get(), sizeof ret);
  return ret;
}
std::unique_ptr<mce::UUID> UnPack(const Mod::proto::UUID &obj) {
  auto ret = std::make_unique<mce::UUID>();
  memcpy(ret.get(), &obj, sizeof obj);
  return std::move(ret);
}
} // namespace flatbuffers