#include "uuid_generated.h"
#include <memory>
#include <type_traits>
#include <vcruntime_string.h>

namespace flatbuffers {
Mod::proto::UUID Pack(mce::UUID const &obj) {
  Mod::proto::UUID ret;
  memcpy(&ret, &obj, sizeof obj);
  return ret;
}
mce::UUID UnPack(const Mod::proto::UUID &obj) {
  mce::UUID ret;
  memcpy(&ret, &obj, sizeof obj);
  return ret;
}
} // namespace flatbuffers