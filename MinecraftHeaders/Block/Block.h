#pragma once

#include "../dll.h"

class Block {
public:
  MCAPI virtual ~Block();
  MCAPI virtual enum BlockRenderLayer getRenderLayer() const;
};