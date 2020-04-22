#include <scriptapi.h>
#include "./lazyapi.h"

using namespace Mod::Scripting;

std::map<std::string, void (*)(JsObjectWrapper global)> &RegisterQueue::GetList() {
  static std::map<std::string, void (*)(JsObjectWrapper global)> root;
  return root;
}

std::map<std::string, std::string (*)(JsObjectWrapper global)> &ModuleRegister::GetList() {
  static std::map<std::string, std::string (*)(JsObjectWrapper global)> root;
  return root;
}

std::map<std::string, std::pair<char const *, std::string (*)(JsObjectWrapper module)>> &LazyModuleRegister::GetList() {
  static std::map<std::string, std::pair<char const *, std::string (*)(JsObjectWrapper module)>> root;
  return root;
}

RegisterQueue::RegisterQueue(char const *name, void (*t)(JsObjectWrapper global)) { GetList()[name] = t; }

ModuleRegister::ModuleRegister(char const *name, std::string (*t)(JsObjectWrapper global)) { GetList()[name] = t; }

LazyModuleRegister::LazyModuleRegister(char const *name, char const *detect, std::string (*t)(JsObjectWrapper global)) {
  GetList()[name] = {detect, t};
}