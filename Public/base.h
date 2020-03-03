#pragma once

#include <Core/mce.h>

#ifdef Base_EXPORTS
#  define BASEAPI __declspec(dllexport)
#else
#  define BASEAPI __declspec(dllimport)
#endif

class DedicatedServer;
class ServiceInstance;
class Minecraft;
class ServerNetworkHandler;
class Level;
namespace RakNet {
class RakPeer;
}

BASEAPI mce::UUID const &SessionUUID();

// Can only be used to get instances of the above types
template <typename T> T *LocateService();

template <> BASEAPI DedicatedServer *LocateService<DedicatedServer>();
template <> BASEAPI ServiceInstance *LocateService<ServiceInstance>();
template <> BASEAPI Minecraft *LocateService<Minecraft>();
template <> BASEAPI ServerNetworkHandler *LocateService<ServerNetworkHandler>();
template <> BASEAPI Level *LocateService<Level>();
template <> BASEAPI RakNet::RakPeer *LocateService<RakNet::RakPeer>();