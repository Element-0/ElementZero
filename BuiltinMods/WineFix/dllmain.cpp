#include <iostream>
#include <hook.h>
#include <processthreadsapi.h>

TClasslessInstanceHook(void, "?leaveGameSync@ServerInstance@@QEAAXXZ") {
  original(this);
  std::cerr << "Quit correctly" << std::endl;
  auto proc = GetCurrentProcess();
  TerminateProcess(proc, 0);
}