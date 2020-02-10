#pragma once

template <typename T> struct typeid_t {
  struct NewIDType {};
  short value;
  static __declspec(dllimport) short count;

  typeid_t(NewIDType) : value(count++) {}
  typeid_t(typeid_t const &id) : value(id.value) {}
};

template <typename T, typename U> typeid_t<T> type_id() {
  static typeid_t<T> id{typename typeid_t<T>::NewIDType()};
  return id;
}