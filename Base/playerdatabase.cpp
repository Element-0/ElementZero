#include <playerdb.h>
#include <log.h>

#include <Core/ExtendedCertificate.h>

DEF_LOGGER("PlayerDB");

template <uint64_t sig> static void (Mod::PlayerDatabase::*emitter)(sigt<sig>, Mod::PlayerEntry const &);

static Mod::PlayerEntryContainer *container;
Mod::PlayerDatabase &db = Mod::PlayerDatabase::GetInstance();

Mod::PlayerDatabase::PlayerDatabase() {
  emitter<"joined"_sig> = &Mod::PlayerDatabase::Emit;
  emitter<"left"_sig>   = &Mod::PlayerDatabase::Emit;
  container             = &data;
}

Mod::PlayerDatabase &Mod::PlayerDatabase::GetInstance() {
  static Mod::PlayerDatabase db;
  return db;
}

TClasslessInstanceHook(
    Player *,
    "?_createNewPlayer@ServerNetworkHandler@@AEAAAEAVServerPlayer@@AEBVNetworkIdentifier@@AEBVConnectionRequest@@@Z",
    void *id, void *req) {
  auto player = original(this, id, req);
  auto &cert  = player->getCertificate();
  auto uuid   = ExtendedCertificate::getIdentity(cert);
  auto name   = ExtendedCertificate::getIdentityName(cert);
  auto xuid   = ExtendedCertificate::getXuid(cert);
  LOGV("%s joined") % name;
  auto ref = container->emplace(Mod::PlayerEntry{player, name, xuid, uuid});
  (db.*emitter<"joined"_sig>) (SIG("joined"), *ref.first);
  return player;
}

TClasslessInstanceHook(
    void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z", Player *player, bool flag) {
  auto it = container->find(player);
  if (it != container->end()) {
    LOGV("%s left") % it->name;
    (db.*emitter<"left"_sig>) (SIG("left"), *it);
    container->erase(it);
  }
  original(this, player, flag);
}