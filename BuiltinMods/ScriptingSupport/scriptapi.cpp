#include <scriptapi.h>
#include "./lazyapi.h"

using namespace Mod::Scripting;

std::map<std::string, void (*)(JsObjectWarpper global)> &RegisterQueue::GetList() {
  static std::map<std::string, void (*)(JsObjectWarpper global)> root;
  return root;
}

std::map<std::string, std::string (*)(JsObjectWarpper global)> &ModuleRegister::GetList() {
  static std::map<std::string, std::string (*)(JsObjectWarpper global)> root;
  return root;
}

std::map<std::string, std::pair<char const *, std::string (*)(JsObjectWarpper module)>> &LazyModuleRegister::GetList() {
  static std::map<std::string, std::pair<char const *, std::string (*)(JsObjectWarpper module)>> root;
  return root;
}

RegisterQueue::RegisterQueue(char const *name, void (*t)(JsObjectWarpper global)) { GetList()[name] = t; }

ModuleRegister::ModuleRegister(char const *name, std::string (*t)(JsObjectWarpper global)) { GetList()[name] = t; }

LazyModuleRegister::LazyModuleRegister(char const *name, char const *detect, std::string (*t)(JsObjectWarpper global)) {
  GetList()[name] = {detect, t};
}