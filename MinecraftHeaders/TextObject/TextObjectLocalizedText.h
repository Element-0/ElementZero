#pragma once

#include "ITextObject.h"

class __declspec(dllimport) TextObjectLocalizedText : public ITextObject {
  std::string text;

public:
  TextObjectLocalizedText(std::string);
  std::string asString() const;
  Json::Value asJsonValue() const;
};