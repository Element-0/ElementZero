#pragma once

#include "ITextObject.h"

#include <vector>

class TextObjectLocalizedTextWithParams : public ITextObject {
public:
  std::string text;
  std::vector<std::string> params;
  TextObjectLocalizedTextWithParams(std::string text, std::initializer_list<std::string> params)
      : text(std::move(text)), params(params) {}

  __declspec(dllimport) std::string asString() const;
  __declspec(dllimport) Json::Value asJsonValue() const;

  inline static auto build(std::string text, std::initializer_list<std::string> params) {
    return std::make_unique<TextObjectLocalizedTextWithParams>(text, params);
  }
};