#pragma once

#include "ITextObject.h"
#include "../dll.h"

class TextObjectText : public ITextObject {
public:
  std::string text;
  inline TextObjectText(std::string text) : text(text) {}

  MCAPI std::string asString() const;
  MCAPI Json::Value asJsonValue() const;
};