#pragma once

#include "../dll.h"
#include <functional>

enum class ActorType { Player = 319 };

enum class ActorTypeNamespaceRules { IncludeNamespace, ExcludeNamespace };

MCAPI void forEachEntityType(std::function<bool(ActorType, std::string const &)>);
MCAPI ActorType EntityTypeFromString(std::string const &);
MCAPI std::string EntityTypeToLocString(ActorType, ActorTypeNamespaceRules);
MCAPI std::string EntityTypeToString(ActorType, ActorTypeNamespaceRules);