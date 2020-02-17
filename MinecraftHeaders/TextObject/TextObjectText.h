#pragma once

#include "ITextObject.h"

class TextObjectText : public ITextObject {
public:
  std::string text;
  inline TextObjectText(std::string text) : text(text) {}

  __declspec(dllimport) std::string asString() const;
  __declspec(dllimport) Json::Value asJsonValue() const;
};