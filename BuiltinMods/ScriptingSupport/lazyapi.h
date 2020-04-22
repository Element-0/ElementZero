#include <map>
#include <string>
#include <utility>

#include <chakra_helper.h>

namespace Mod::Scripting {

struct LazyModuleRegister {
  static std::map<std::string, std::pair<char const *, std::string (*)(JsObjectWrapper module)>> &GetList();

  LazyModuleRegister(char const *name, char const *detect, std::string (*t)(JsObjectWrapper module));
};



} // namespace Mod::Scripting