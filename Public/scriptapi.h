#pragma once

#include <string>
#include <map>

#include "chakra_helper.h"

#ifdef ScriptingSupport_EXPORTS
#  define SCRIPTAPI __declspec(dllexport)
#else
#  define SCRIPTAPI __declspec(dllimport)
#endif

struct RegisterQueue {
  SCRIPTAPI static std::map<std::string, void (*)(JsObjectWarpper global)> &GetList();

  SCRIPTAPI RegisterQueue(char const *name, void (*t)(JsObjectWarpper global));
};

struct ModuleRegister {
  SCRIPTAPI static std::map<std::string, std::string (*)(JsObjectWarpper module)> &GetList();

  SCRIPTAPI ModuleRegister(char const *name, std::string (*t)(JsObjectWarpper global));
};