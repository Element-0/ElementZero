#include <base.h>
#include <Actor/Player.h>
#include <Command/CommandOutput.h>
#include <Item/Item.h>

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

void CommandOutput::success() { direct_access<bool>(this, 40) = true; }

template <> Minecraft *LocateService<Minecraft>() {
  return direct_access<Minecraft *>(LocateService<ServiceInstance>(), 32);
}

bool Item::getAllowOffhand() const { return direct_access<char>(this, 258) & 0x40; }