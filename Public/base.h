#pragma once

#ifdef Base_EXPORTS
#  define BASEAPI __declspec(dllexport)
#else
#  define BASEAPI __declspec(dllimport)
#endif

class DedicatedServer;
class ServiceInstance;
class Minecraft;
class Level;

template <typename T> T *LocateService();

template <> BASEAPI DedicatedServer *LocateService<DedicatedServer>();
template <> BASEAPI ServiceInstance *LocateService<ServiceInstance>();
template <> BASEAPI Minecraft *LocateService<Minecraft>();
template <> BASEAPI Level *LocateService<Level>();