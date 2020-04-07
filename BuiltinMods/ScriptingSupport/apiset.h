#pragma once

#include <string>
#include <map>

#include "chakra_helper.h"

struct RegisterQueue {
  static std::map<std::string, void (*)(JsObjectWarpper global)> &GetList();

  RegisterQueue(char const *name, void (*t)(JsObjectWarpper global));
};

struct ModuleRegister {
  static std::map<std::string, void (*)(JsModuleRecord module)> &GetList();

  inline ModuleRegister(char const *name, void (*t)(JsModuleRecord global));
};