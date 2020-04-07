#pragma once

#include <string>
#include <map>

#include "chakra_helper.h"

class RegisterQueue {
public:
  inline static std::map<std::string, std::function<void(JsObjectWarpper global)>> &GetList() {
    static std::map<std::string, std::function<void(JsObjectWarpper global)>> root;
    return root;
  }

  template <typename T> RegisterQueue(char const *name, T t) { RegisterQueue::GetList()[name] = t; }
};