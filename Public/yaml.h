#pragma once

#include <yaml-cpp/yaml.h>

template <typename T> inline bool yaml_assign(T &target, YAML::Node node) {
  return YAML::convert<T>::decode(node, target);
}