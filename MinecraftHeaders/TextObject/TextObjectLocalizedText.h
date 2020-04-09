#pragma once

#include "ITextObject.h"
#include "../dll.h"

class TextObjectLocalizedText : public ITextObject {
  std::string text;

public:
  MCAPI TextObjectLocalizedText(std::string);
  MCAPI std::string asString() const;
  MCAPI Json::Value asJsonValue() const;
};