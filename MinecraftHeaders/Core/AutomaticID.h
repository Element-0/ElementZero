#pragma once

template <typename Type, typename Store> class AutomaticID {
public:
  Store value;
  // make it non-POD
  inline ~AutomaticID() {}
};