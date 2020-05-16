
#include <memory>
#include <stdexcept>

#include <Item/ItemStack.h>
#include <Item/Item.h>
#include <Item/VanillaItems.h>
#include <Core/NBT.h>
#include <Packet/TextPacket.h>
#include <Actor/Player.h>
#include <Block/VanillaBlockTypes.h>
#include <Block/BlockLegacy.h>
#include <Block/BlockSource.h>
#include <BlockActor/BlockActorType.h>
#include <BlockActor/Sign.h>

#include <dllentry.h>
#include <hook.h>
#include <base.h>
#include <command.h>
#include <log.h>

DEF_LOGGER("SignCommand");

void dllenter() {}
void dllexit() {}

struct MySignBlockActor : SignBlockActor {
  using SignBlockActor::SignBlockActor;

  inline static std::string const TAG_COMMAND = "Command";

  std::string command;

  void load(class Level &level, const class CompoundTag &tag, class DataLoadHelper &helper) override {
    SignBlockActor::load(level, tag, helper);
    if (tag.contains(TAG_COMMAND, Tag::String)) command = tag.getString(TAG_COMMAND);
  }
  bool save(class CompoundTag &tag) const override {
    auto ret = SignBlockActor::save(tag);
    if (ret)
      if (!command.empty()) tag.putString(TAG_COMMAND, command);
    return ret;
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

void PreInit() {
  VTableHook::Create<&BlockLegacy::use>("??_7SignBlock@@6B@")
      ->Replace(
          "?use@BlockLegacy@@UEBA_NAEAVPlayer@@AEBVBlockPos@@@Z",
          +[](BlockLegacy *self, class Player &player, const class BlockPos &pos) {
            if (auto ba = player.Region->getBlockEntity(pos))
              if (auto me = dynamic_cast<MySignBlockActor *>(ba)) {
                if (player.canUseOperatorBlocks()) {
                  auto &stack = player.getSelectedItem();
                  if (stack.Id == VanillaItems::mStick->Id) {
                    me->command = stack.CustomName;
                    auto pkt    = TextPacket::createTranslatedMessageWithParams("signcommand.set");
                    player.sendNetworkPacket(pkt);
                    return true;
                  }
                }
                if (me->command.empty()) return true;
                auto orig           = std::make_unique<Mod::CustomCommandOrigin>();
                orig->name          = "sign";
                orig->pos           = pos;
                orig->worldPosition = player.getPos();
                orig->dim           = player.Dimension;
                orig->actor         = &player;
                Mod::CommandSupport::GetInstance().ExecuteCommand(std::move(orig), me->command);
              }
            return true;
          });
}