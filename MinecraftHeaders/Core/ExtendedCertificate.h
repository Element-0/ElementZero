#pragma once

#include <string>
#include "../Core/mce.h"
#include "../dll.h"

class Certificate;

class ExtendedCertificate {
public:
  static MCAPI std::string getXuid(Certificate const &);
  static MCAPI mce::UUID getIdentity(Certificate const &);
  static MCAPI std::string getIdentityName(Certificate const &);
};