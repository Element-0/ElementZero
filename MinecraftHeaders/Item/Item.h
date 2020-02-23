#pragma once

#include <hook.h>

#ifndef BASEAPI
#  define BASEAPI __declspec(dllimport)
#endif

class Item {
public:
  BASEAPI bool getAllowOffhand() const;
};