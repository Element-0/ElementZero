#pragma once

#include <vector>
#include <memory>

#include "ITextObject.h"
#include "../dll.h"

class TextObjectRoot : public ITextObject {
  std::vector<std::unique_ptr<ITextObject>> children;

public:
  inline TextObjectRoot() {}
  MCAPI std::string asString() const;
  MCAPI Json::Value asJsonValue() const;

  MCAPI void addChild(std::unique_ptr<ITextObject>);
  MCAPI void clear();
  MCAPI bool isEmpty() const;
};