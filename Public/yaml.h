#pragma once

#include <yaml-cpp/yaml.h>

#include <type_traits>
#include <boost/type_traits/is_detected.hpp>

template <typename T>[[deprecated]] inline bool yaml_assign(T &target, YAML::Node const &node) {
  try {
    return YAML::convert<T>::decode(node, target);
  } catch (...) { return false; }
}

template <bool IsSave> struct YamlIO;

template <typename T> bool ReadYAML(T &target, YAML::Node const &node) {
  return T::io(YamlIO<false>{}, target, const_cast<YAML::Node &>(node));
}

template <typename T> bool WriteYAML(T &target, YAML::Node &node) {
  return T::io(YamlIO<true>{}, target, node);
}

template <typename T> using has_io = decltype(T::io(std::declval<YamlIO<false>>(), std::declval<T &>(), std::declval<YAML::Node &>()));

template <> struct YamlIO<false> {
  template <typename T> inline bool operator()(T &target, YAML::Node const &node) {
    try {
      if constexpr (boost::is_detected_v<has_io, T>) {
        return ReadYAML(target, node);
      } else {
        return YAML::convert<T>::decode(node, target);
      }
    } catch (...) { return false; }
  }
};

template <> struct YamlIO<true> {
  template <typename T> inline bool operator()(T &target, YAML::Node &&node) {
    try {
      if constexpr (boost::is_detected_v<has_io, T>) {
        return WriteYAML(target, node);
      } else {
        node = YAML::convert<T>::encode(target);
      }
      return true;
    } catch (...) { return false; }
  }
};