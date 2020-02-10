#pragma once

#ifdef BASE_EXPORTS
#  define BASEAPI __declspec(dllexport)
#else
#  define BASEAPI __declspec(dllimport)
#endif

class DedicatedServer;
extern "C" BASEAPI DedicatedServer *GetDedicatedServer();