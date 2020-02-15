#pragma once

#include <string>
#include "../Core/mce.h"

class Certificate;

class ExtendedCertificate {
public:
  static __declspec(dllimport) std::string getXuid(Certificate const &);
  static __declspec(dllimport) mce::UUID getIdentity(Certificate const &);
  static __declspec(dllimport) std::string getIdentityName(Certificate const &);
};