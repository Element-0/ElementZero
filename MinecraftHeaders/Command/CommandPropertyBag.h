#pragma once

#include <string>

#include "PropertyBag.h"
#include "../dll.h"

class CommandPropertyBag : public PropertyBag {
public:
  virtual ~CommandPropertyBag(){};

  MCAPI void addToResultList(std::string const &key, std::string const &value);
};