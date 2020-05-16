#include <Core/NBT.h>
#include <Actor/Player.h>
#include <Block/VanillaBlockTypes.h>
#include <Block/BlockLegacy.h>
#include <Block/BlockSource.h>
#include <BlockActor/BlockActorType.h>
#include <BlockActor/Sign.h>
#include <dllentry.h>
#include <hook.h>
#include <base.h>
#include <log.h>
#include <memory>
#include <stdexcept>
#include <vcruntime.h>

DEF_LOGGER("BlockMod");

void dllenter() {}
void dllexit() {}

void hexDump(const char *desc, const void *addr, const int len) {
  int i;
  unsigned char buff[17];
  const unsigned char *pc = (const unsigned char *) addr;

  if (desc != NULL) printf("%s:\n", desc);

  if (len == 0) {
    printf("  ZERO LENGTH\n");
    return;
  } else if (len < 0) {
    printf("  NEGATIVE LENGTH: %d\n", len);
    return;
  }

  for (i = 0; i < len; i++) {
    if ((i % 16) == 0) {
      if (i != 0) printf("  %s\n", buff);

      printf("  %04x ", i);
    }

    printf(" %02x", pc[i]);
    if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
      buff[i % 16] = '.';
    else
      buff[i % 16] = pc[i];
    buff[(i % 16) + 1] = '\0';
  }

  while ((i % 16) != 0) {
    printf("   ");
    i++;
  }
  printf("  %s\n", buff);
}

template <size_t len, typename T> void printlam(T t) {
  union {
    T src;
    void *arr;
  } u;
  u.src = t;
  hexDump(nullptr, u.arr, 32);
}

struct FailedToReplaceVTable : std::runtime_error {
  using runtime_error::runtime_error;
};

struct MySignBlockActor : SignBlockActor {
  using SignBlockActor::SignBlockActor;

  std::string command;
  void load(class Level &level, const class CompoundTag &tag, class DataLoadHelper &helper) override {
    SignBlockActor::load(level, tag, helper);
    if (tag.contains("Command", Tag::String)) command = tag.getString("Command");
  }
  bool save(class CompoundTag &tag) const override {
    if (!command.empty()) tag.putString("Command", command);
    return SignBlockActor::save(tag);
  }
};

THook(
    std::shared_ptr<BlockActor>,
    "?createBlockEntity@BlockActorFactory@@SA?AV?$shared_ptr@VBlockActor@@@std@@W4BlockActorType@@AEBVBlockPos@@"
    "AEBVBlockLegacy@@@Z",
    enum BlockActorType type, class BlockPos const &pos, class BlockLegacy const &block) {
  if (type == BlockActorType::Sign)
    return std::static_pointer_cast<BlockActor>(std::make_shared<MySignBlockActor>(pos));
  return original(type, pos, block);
}

template <typename T, auto... ts> T create() { return {ts...}; }

void PreInit() {
  LOGV("%s") % typeid(&BlockLegacy::use).raw_name();
  printlam<32>(&BlockLegacy::use);
  printlam<32>(&BlockLegacy::tick);

  LOGV("offset: %p") % GetVTableOffset<&BlockLegacy::use>();
  LOGV("offset: %p") % GetVTableOffset<&BlockLegacy::tick>();
  auto vtable   = GetServerSymbol<void **>("??_7SignBlock@@6B@");
  auto targetfn = GetServerSymbol("?use@BlockLegacy@@UEBA_NAEAVPlayer@@AEBVBlockPos@@@Z");
  LOGV("exp: %p") % targetfn;
  LOGV("act: %p") % vtable[GetVTableOffset<&BlockLegacy::use>()];
  for (auto it = vtable; it; ++it) {
    if (*it == targetfn) {
      LOGV("Found: %p!\n") % (it - vtable);
      break;
    }
  }
  VTableHook::Create<&BlockLegacy::use>("??_7SignBlock@@6B@")
      ->Replace(
          "?use@BlockLegacy@@UEBA_NAEAVPlayer@@AEBVBlockPos@@@Z",
          +[](BlockLegacy *self, class Player &player, const class BlockPos &pos) {
            if (auto ba = player.Region->getBlockEntity(pos))
              if (auto me = dynamic_cast<MySignBlockActor *>(ba)) {
                //
              }
            return true;
          });
}