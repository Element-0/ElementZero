#pragma once

#include <yaml.h>
#include <cstdint>

struct Settings {
  struct PosXY {
    int x = 0, z = 0;

    template <typename IO> static inline bool io(IO f, PosXY &xy, YAML::Node &node) {
      return f(xy.x, node["x"]) && f(xy.z, node["z"]);
    }
  };

  bool AllowOperator = false;
  PosXY Start, End;

  template <typename IO> static inline bool io(IO f, Settings &self, YAML::Node &node) {
    return f(self.AllowOperator, node["allow-operator"]) && f(self.Start, node["start"]) && f(self.End, node["end"]);
  }
};

extern Settings settings;