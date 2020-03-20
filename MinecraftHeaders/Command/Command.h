#pragma once

#include <string>
#include "CommandFlag.h"

class CommandRegistry;
class CommandOrigin;
class CommandOutput;
template <typename T> class CommandSelectorResults;

class Command {
protected:
  int unk8;         // 8
  void *unk16;      // 16
  int unk24;        // 24
  char b28;         // 28
  CommandFlag flag; // 29
  Command() {
    unk24 = -1;
    b28   = 5;
  }

public:
  __declspec(dllimport) virtual ~Command();
  virtual void execute(CommandOrigin const &, CommandOutput &) = 0;

protected:
  //??$checkHasTargets@VActor@@@Command@@KA_NAEBV?$CommandSelectorResults@VActor@@@@AEAVCommandOutput@@@Z
  template <typename T>
  __declspec(dllimport) static bool checkHasTargets(CommandSelectorResults<T> const &, CommandOutput &);
};