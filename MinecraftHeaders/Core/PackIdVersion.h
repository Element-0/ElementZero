#pragma once

#include "mce.h"
#include "SemVersion.h"

enum struct PackType : char { ResourcePack, BehaviorPack };

struct PackIdVersion {
  mce::UUID uuid;
  SemVersion version;
  PackType type;
};