#pragma once

#include <string>
#include <cstdint>

#include "../dll.h"

class HashedString {
  uint64_t hash;
  std::string str;

public:
  std::string const &getString() const { return str; }

  MCAPI HashedString(HashedString const &rhs);
  MCAPI HashedString(HashedString &&rhs);
  MCAPI HashedString(char const *rhs);
  MCAPI HashedString(std::string const &rhs);
  MCAPI bool operator==(HashedString const &rhs) const;
  MCAPI bool operator!=(HashedString const &rhs) const;
};