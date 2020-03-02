#pragma once

#include <thread>

namespace Bedrock::Threading {

__declspec(dllimport) extern std::thread::id gMainThread;
__declspec(dllimport) extern std::thread::id gServerThread;

} // namespace Bedrock::Threading