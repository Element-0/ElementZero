#pragma once

#include <string>

namespace Json {
class Value;
}

class __declspec(dllimport) ITextObject {
public:
  virtual ~ITextObject();
  virtual std::string asString() const = 0;
  virtual Json::Value asJsonValue() const = 0;
};