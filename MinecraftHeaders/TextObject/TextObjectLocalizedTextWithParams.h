#pragma once

#include <vector>
#include <memory>

#include "ITextObject.h"
#include "../dll.h"

class TextObjectLocalizedTextWithParams : public ITextObject {
public:
  std::string text;
  std::vector<std::string> params;
  TextObjectLocalizedTextWithParams(std::string text, std::initializer_list<std::string> params)
      : text(std::move(text)), params(params) {}

  MCAPI std::string asString() const;
  MCAPI Json::Value asJsonValue() const;

  inline static auto build(std::string text, std::initializer_list<std::string> params) {
    return std::make_unique<TextObjectLocalizedTextWithParams>(text, params);
  }
};