#pragma once

#include <string>

namespace Json {
class Value;
}

class ITextObject {
public:
  __declspec(dllimport) virtual ~ITextObject();
  virtual std::string asString() const = 0;
  virtual Json::Value asJsonValue() const = 0;
};