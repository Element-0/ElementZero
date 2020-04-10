#pragma once

#include <hook.h>
#include "../Math/Vec3.h"
#include "../Core/AutomaticID.h"

class Dimension;

class Actor {
public:
  inline std::string getEntityName() const {
    return CallServerFunction<std::string>(
        "?getEntityName@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVActor@@@Z", this);
  }
  inline Vec3 const &getPos() const {
    return CallServerClassMethod<Vec3 const &>("?getPos@Actor@@UEBAAEBVVec3@@XZ", this);
  }
  inline Vec3 getFiringPos() const { return CallServerClassMethod<Vec3>("?getFiringPos@Actor@@UEBA?AVVec3@@XZ", this); }
  inline std::string const &getNameTag() const {
    return CallServerClassMethod<std::string const &>(
        "?getNameTag@Actor@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", this);
  }
  inline AutomaticID<Dimension, int> getDimensionId() const {
    return CallServerClassMethod<AutomaticID<Dimension, int>>(
        "?getDimensionId@Actor@@UEBA?AV?$AutomaticID@VDimension@@H@@XZ", this);
  }

  inline bool getAlwaysShowNameTag() const {
      return CallServerClassMethod<bool>(
          "?getAlwaysShowNameTag@Actor@@UEBA_NXZ", this);
  }

  inline enum ActorType getEntityTypeId() const {
      return CallServerClassMethod<enum ActorType>(
          "?getEntityTypeId@Actor@@UEBA?AW4ActorType@@XZ", this);
  }

  void teleport(Vec3 const &target, Vec3 const &old, AutomaticID<Dimension, int> dim) {
    CallServerClassMethod<void>(
        "?teleport@TeleportCommand@@AEBAXAEAVActor@@VVec3@@PEAV3@V?$AutomaticID@VDimension@@H@@@Z", this, this,
        target, &old, dim);
  }
};

class Mob : public Actor {
public:
    inline void sendInventory(bool a0) const {
        return CallServerClassMethod<void>(
            "?sendInventory@Mob@@UEAAX_N@Z", this, a0);
    }
};