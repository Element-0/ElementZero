#include <Actor/Player.h>

#include <dllentry.h>
#include <hook.h>
#include <yaml.h>

void dllenter() {}
void dllexit() {}

struct Settings {
  bool bypass_op = false;

  template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
    return f(settings.bypass_op, node["bypass_op"]);
  }
};

inline Settings settings;

DEFAULT_SETTINGS(settings);

TClasslessInstanceHook(void, "?save@LevelStorage@@QEAAXAEAVActor@@@Z", Player &player) {
  if (settings.bypass_op && player.getCommandPermissionLevel() > CommandPermissionLevel::Any) original(this, player);
}