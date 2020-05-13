#pragma once

#include "../Core/WeakPtr.h"
#include "../Core/NBT.h"
#include "BlockLegacy.h"
#include "../dll.h"

class Block {
public:
  unsigned short aux;
  WeakPtr<BlockLegacy> legacy;
  CompoundTag definition;
  int runtime_id;
  bool has_runtime_id;

  MCAPI virtual ~Block();
  MCAPI virtual enum BlockRenderLayer getRenderLayer() const;
};