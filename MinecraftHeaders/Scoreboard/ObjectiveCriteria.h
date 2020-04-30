#pragma once

#include <memory>
#include <string>

#include "../Core/NBT.h"
#include "ObjectiveRenderType.h"
#include "../dll.h"

class ObjectiveCriteria {
public:
  std::string name;
  bool readonly;
  ObjectiveRenderType type;

  MCAPI static std::unique_ptr<ObjectiveCriteria> deserialize(CompoundTag const &);
};