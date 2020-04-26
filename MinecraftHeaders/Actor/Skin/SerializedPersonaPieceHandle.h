#pragma once

#include "persona.h"
#include "../../Core/mce.h"

class SerializedPersonaPieceHandle {
public:
  std::string name;
  PieceType type;
  mce::UUID uuid;
  bool flag;
  std::string data;
};