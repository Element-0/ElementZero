#include <base.h>
#include <Actor/Player.h>

template <typename Ret, typename Type> Ret &direct_access(Type *type, size_t offset) {
  union {
    size_t raw;
    Type *source;
    Ret *target;
  } u;
  u.source = type;
  u.raw += offset;
  return *u.target;
}

Certificate &Player::getCertificate() { return *direct_access<Certificate *>(this, 3208); }