#pragma once

#include <string>
#include <map>

#include "chakra_helper.h"

struct RegisterQueue {
  static std::map<std::string, void (*)(JsObjectWarpper global)> &GetList();

  RegisterQueue(char const *name, void (*t)(JsObjectWarpper global));
};

struct ModuleRegister {
  static std::map<std::string, std::string (*)(JsObjectWarpper module)> &GetList();

  ModuleRegister(char const *name, std::string (*t)(JsObjectWarpper global));
};