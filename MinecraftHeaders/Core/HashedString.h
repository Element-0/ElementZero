#pragma once

#include <string>
#include <cstdint>

class HashedString {
  uint64_t hash;
  std::string str;

public:
  std::string const &getString() const { return str; }

  __declspec(dllimport) HashedString(HashedString const &rhs);
  __declspec(dllimport) HashedString(HashedString &&rhs);
  __declspec(dllimport) HashedString(char const *rhs);
  __declspec(dllimport) HashedString(std::string const &rhs);
  __declspec(dllimport) bool operator==(HashedString const &rhs) const;
  __declspec(dllimport) bool operator!=(HashedString const &rhs) const;
};