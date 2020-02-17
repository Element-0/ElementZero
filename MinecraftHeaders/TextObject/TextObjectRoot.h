#pragma once

#include "ITextObject.h"

#include <vector>
#include <memory>

class TextObjectRoot : public ITextObject {
  std::vector<std::unique_ptr<ITextObject>> children;

public:
  inline TextObjectRoot() {}
  __declspec(dllimport) std::string asString() const;
  __declspec(dllimport) Json::Value asJsonValue() const;

  __declspec(dllimport) void addChild(std::unique_ptr<ITextObject>);
  __declspec(dllimport) void clear();
  __declspec(dllimport) bool isEmpty() const;
};