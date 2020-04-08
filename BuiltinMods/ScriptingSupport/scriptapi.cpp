#include <scriptapi.h>

std::map<std::string, void (*)(JsObjectWarpper global)> &RegisterQueue::GetList() {
  static std::map<std::string, void (*)(JsObjectWarpper global)> root;
  return root;
}

std::map<std::string, std::string (*)(JsObjectWarpper global)> &ModuleRegister::GetList() {
  static std::map<std::string, std::string (*)(JsObjectWarpper global)> root;
  return root;
}

RegisterQueue::RegisterQueue(char const *name, void (*t)(JsObjectWarpper global)) {
  RegisterQueue::GetList()[name] = t;
}

ModuleRegister::ModuleRegister(char const *name, std::string (*t)(JsObjectWarpper global)) {
  ModuleRegister::GetList()[name] = t;
}