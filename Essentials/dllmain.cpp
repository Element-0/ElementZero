#include "pch.h"
#include "global.h"
#include <dllentry.h>

Settings settings;

namespace YAML {
template <> struct convert<Settings::Commands> {
  static bool decode(const Node &node, Settings::Commands &rhs) {
    yaml_assign(rhs.transferserver, node["transferserver"]);
    return true;
  }
};
template <> struct convert<Settings> {
  static bool decode(const Node &node, Settings &rhs) {
    yaml_assign(rhs.commands, node["commands"]);
    yaml_assign(rhs.force_experimental_gameplay, node["force-experimental-gameplay"]);
    yaml_assign(rhs.education_feature, node["education-feature"]);
    yaml_assign(rhs.debug_packs, node["debug-packs"]);
    return true;
  }
};
} // namespace YAML

extern "C" __declspec(dllexport) void ApplySettings(YAML::Node const &node) {
  DEF_LOGGER("SETTINGS");
  yaml_assign(settings, node);
  if (settings.force_experimental_gameplay) {
    LOGI("Force experimental gameplay enabled");
    *GetServerSymbol<bool>("?mAllowExperimental@Enchant@@1_NA") = true;
    *GetServerSymbol<bool>("?mAllowExperimental@Item@@2_NA")    = true;
  }
}

void dllenter() { Mod::CommandSupport::GetInstance().AddListener(SIG("loaded"), startRegister); }
void dllexit() {}