#pragma once

#include "../TextObject/TextObjectRoot.h"
#include "BlockActor.h"
#include "../dll.h"

#include <modutils.h>

class SignBlockActor : public BlockActor {
  char pad[352];

public:
  BUILD_ACCESS(TextObjectRoot, text_object, 304);
  BUILD_ACCESS(std::string, text_single, 240);
  BUILD_ACCESS(std::string, text_owner, 208);

  MCAPI SignBlockActor(class BlockPos const &);
  MCAPI virtual void load(class Level &, class CompoundTag const &, class DataLoadHelper &) override;
  MCAPI virtual bool save(class CompoundTag &) const override;
  MCAPI virtual void onChanged(class BlockSource &) override;
  MCAPI virtual std::unique_ptr<class Packet> getUpdatePacket(class BlockSource &) override;
  MCAPI virtual float getShadowRadius(class BlockSource &) const override;
  MCAPI virtual std::string getImmersiveReaderText(class BlockSource &) override;
  MCAPI virtual void _onUpdatePacket(class CompoundTag const &, class BlockSource &) override;

  MCAPI void setMessage(TextObjectRoot text, std::string owner);
  MCAPI void setMessage(std::string text, std::string owner);
};

static_assert(sizeof(SignBlockActor) == 560);