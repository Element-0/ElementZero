#pragma once

#include <vector>
#include <string>

class GameRule {
public:
  enum struct Type : char { Invalid = 0, Boolean = 1, Integer = 2, Float = 3 };
  union Value {
    bool val_bool;
    int val_int;
    float val_float;
  };

  bool saved;
  Type type;
  Value value;
  std::string name;
  bool useInCommand;
  bool defaultSet;
};

class GameRules {
  std::vector<GameRule> rules;
};