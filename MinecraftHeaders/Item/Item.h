#pragma once

#include <memory>
#include <string>

#include "../Core/json.h"
#include "../dll.h"

#include <hook.h>
#include <base.h>
#include <modutils.h>

enum class BlockShape;
enum class CooldownType;
enum class UseAnimation;
enum class CreativeItemCategory;
enum class InHandUpdateType;
enum class ItemUseMethod;

class Item {
public:
  MCAPI virtual ~Item();
  MCAPI virtual void tearDown();
  MCAPI virtual int getMaxUseDuration(class ItemInstance const *) const;
  MCAPI virtual int getMaxUseDuration(class ItemStack const *) const;
  MCAPI virtual bool isExperimental(class ItemDescriptor const *) const;
  MCAPI virtual class Item &setMaxStackSize(unsigned char);
  MCAPI virtual class Item &setCategory(enum CreativeItemCategory);
  MCAPI virtual class Item &setStackedByData(bool);
  MCAPI virtual class Item &setMaxDamage(int);
  MCAPI virtual class Item &setHandEquipped();
  MCAPI virtual class Item &setUseAnimation(enum UseAnimation);
  MCAPI virtual class Item &setMaxUseDuration(int);
  MCAPI virtual class Item &setRequiresWorldBuilder(bool);
  MCAPI virtual class Item &setExplodable(bool);
  MCAPI virtual class Item &setIsGlint(bool);
  MCAPI virtual class Item &setShouldDespawn(bool);
  MCAPI virtual enum BlockShape getBlockShape() const;
  MCAPI virtual bool canBeDepleted() const;
  MCAPI virtual bool canDestroySpecial(class Block const &) const;
  MCAPI virtual int getLevelDataForAuxValue(int) const;
  MCAPI virtual bool isStackedByData() const;
  MCAPI virtual short getMaxDamage() const;
  MCAPI virtual int getAttackDamage() const;
  MCAPI virtual bool isHandEquipped() const;
  MCAPI virtual bool isArmor() const;
  MCAPI virtual bool isDye() const;
  MCAPI virtual bool isFertilizer(int) const;
  MCAPI virtual bool isGlint(class ItemStackBase const &) const;
  MCAPI virtual bool isThrowable() const;
  MCAPI virtual bool isPattern() const;
  MCAPI virtual bool showsDurabilityInCreative() const;
  MCAPI virtual bool isWearableThroughLootTable(std::unique_ptr<class CompoundTag> const &) const;
  MCAPI virtual bool canDestroyInCreative() const;
  MCAPI virtual bool isDestructive(int) const;
  MCAPI virtual bool isLiquidClipItem(int) const;
  MCAPI virtual bool requiresInteract() const;
  MCAPI virtual void appendFormattedHovertext(class ItemStackBase const &, class Level &, std::string &, bool) const;
  MCAPI virtual bool isValidRepairItem(class ItemInstance const &, class ItemInstance const &) const;
  MCAPI virtual int getEnchantSlot() const;
  MCAPI virtual int getEnchantValue() const;
  MCAPI virtual int getArmorValue() const;
  MCAPI virtual bool isComplex() const;
  MCAPI virtual bool isValidAuxValue(int) const;
  MCAPI virtual int getDamageChance(int) const;
  MCAPI virtual bool uniqueAuxValues() const;
  MCAPI virtual bool isMultiColorTinted(class ItemStack const &) const;
  MCAPI virtual class Color getColor(std::unique_ptr<class CompoundTag> const &, class ItemDescriptor const &) const;
  MCAPI virtual class Color getBaseColor(class ItemStack const &) const;
  MCAPI virtual class Color getSecondaryColor(class ItemStack const &) const;
  MCAPI virtual void saveAdditionalData(class ItemStackBase const &, class CompoundTag &) const;
  MCAPI virtual void readAdditionalData(class ItemStackBase &, class CompoundTag const &) const;
  MCAPI virtual bool isTintable() const;
  MCAPI virtual class ItemStack &use(class ItemStack &, class Player &) const;
  MCAPI virtual bool dispense(class BlockSource &, class Container &, int, class Vec3 const &, unsigned char) const;
  MCAPI virtual enum ItemUseMethod useTimeDepleted(class ItemInstance &, class Level *, class Player *) const;
  MCAPI virtual enum ItemUseMethod useTimeDepleted(class ItemStack &, class Level *, class Player *) const;
  MCAPI virtual void releaseUsing(class ItemInstance &, class Player *, int) const;
  MCAPI virtual void releaseUsing(class ItemStack &, class Player *, int) const;
  MCAPI virtual float getDestroySpeed(class ItemInstance const &, class Block const &) const;
  MCAPI virtual void hurtEnemy(class ItemInstance &, class Mob *, class Mob *) const;
  MCAPI virtual void hurtEnemy(class ItemStack &, class Mob *, class Mob *) const;
  MCAPI virtual bool mineBlock(class ItemInstance &, class Block const &, int, int, int, class Actor *) const;
  MCAPI virtual bool mineBlock(class ItemStack &, class Block const &, int, int, int, class Actor *) const;
  MCAPI virtual std::string
  buildDescriptionId(class ItemDescriptor const &, std::unique_ptr<class CompoundTag> const &) const;
  MCAPI virtual std::string buildEffectDescriptionName(class ItemStackBase const &) const;
  MCAPI virtual std::string buildCategoryDescriptionName() const;
  MCAPI virtual void readUserData(class ItemStackBase &, class IDataInput &, class ReadOnlyBinaryStream &) const;
  MCAPI virtual void writeUserData(class ItemStackBase const &, class IDataOutput &) const;
  MCAPI virtual unsigned char getMaxStackSize(class ItemDescriptor const &) const;
  MCAPI virtual bool inventoryTick(class ItemStack &, class Level &, class Actor &, int, bool) const;
  MCAPI virtual void refreshedInContainer(class ItemStackBase &, class Level &) const;
  MCAPI virtual enum CooldownType getCooldownType() const;
  MCAPI virtual int getCooldownTime() const;
  MCAPI virtual void fixupOnLoad(class ItemStackBase &) const;
  MCAPI virtual void fixupOnLoad(class ItemStackBase &, class Level &) const;
  MCAPI virtual short getDamageValue(std::unique_ptr<class CompoundTag> const &) const;
  MCAPI virtual void setDamageValue(class ItemStackBase &, short) const;
  MCAPI virtual enum InHandUpdateType
  getInHandUpdateType(class Player const &, class ItemInstance const &, class ItemInstance const &, bool, bool) const;
  MCAPI virtual enum InHandUpdateType
  getInHandUpdateType(class Player const &, class ItemStack const &, class ItemStack const &, bool, bool) const;
  MCAPI virtual bool isSameItem(class ItemStackBase const &, class ItemStackBase const &) const;
  MCAPI virtual std::string getInteractText(class Player const &) const;
  MCAPI virtual int getAnimationFrameFor(class Mob *, bool, class ItemStack const *, bool) const;
  MCAPI virtual bool isEmissive(int) const;
  MCAPI virtual struct TextureUVCoordinateSet const &getIcon(class ItemStackBase const &, int, bool) const;
  MCAPI virtual int getIconYOffset() const;
  MCAPI virtual class Item &setIcon(std::string const &, int);
  MCAPI virtual class Item &setIcon(struct TextureUVCoordinateSet const &);
  MCAPI virtual class Item &setIconAtlas(std::string const &, int);
  MCAPI virtual bool canBeCharged() const;
  MCAPI virtual void playSoundIncrementally(class ItemInstance const &, class Mob &) const;
  MCAPI virtual void playSoundIncrementally(class ItemStack const &, class Mob &) const;
  MCAPI virtual std::string getAuxValuesDescription() const;
  MCAPI virtual bool
  _checkUseOnPermissions(class Actor &, class ItemInstance &, unsigned char const &, class BlockPos const &) const;
  MCAPI virtual bool
  _checkUseOnPermissions(class Actor &, class ItemStack &, unsigned char const &, class BlockPos const &) const;
  MCAPI virtual bool _calculatePlacePos(class ItemInstance &, class Actor &, unsigned char &, class BlockPos &) const;
  MCAPI virtual bool _calculatePlacePos(class ItemStack &, class Actor &, unsigned char &, class BlockPos &) const;
  MCAPI virtual bool
  _useOn(class ItemInstance &, class Actor &, class BlockPos, unsigned char, float, float, float) const;
  MCAPI virtual bool _useOn(class ItemStack &, class Actor &, class BlockPos, unsigned char, float, float, float) const;

  MCAPI class Item &setAllowOffhand(bool);
  MCAPI class Item &setIsMirroredArt(bool);
  MCAPI bool initServer(class Json::Value &);
  MCAPI bool useOn(class ItemStack &, class Actor &, int, int, int, unsigned char, float, float, float) const;
  MCAPI class Item &setMinRequiredBaseGameVersion(class BaseGameVersion const &);
  MCAPI bool updateCustomBlockEntityTag(class BlockSource &, class ItemStack &, class BlockPos &) const;
  MCAPI std::string const &getCommandName() const;
  MCAPI std::string getSerializedName() const;
  MCAPI std::string buildDescriptionName(class ItemStackBase const &) const;

  BASEAPI bool getAllowOffhand() const;
  DEF_FIELD_RW(bool, AllowOffhand);
};