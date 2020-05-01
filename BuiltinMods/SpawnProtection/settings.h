#pragma once

#include <yaml.h>
#include <cstdint>

struct Settings {
  struct PosXZ {
    int x = 0, z = 0;

    template <typename IO> static inline bool io(IO f, PosXZ &sz, YAML::Node &node) {
      return f(sz.x, node["x"]) && f(sz.z, node["z"]);
    }
  };

  bool AllowOperator = false;
  PosXZ Start, End;

  template <typename IO> static inline bool io(IO f, Settings &self, YAML::Node &node) {
    return f(self.AllowOperator, node["allow-operator"]) && f(self.Start, node["start"]) && f(self.End, node["end"]);
  }
};

extern Settings settings;

enum struct Mode { Disabled, Enforce, Permissive };
extern Mode mode;

class CommandRegistry;
void initCommand(CommandRegistry *registry);