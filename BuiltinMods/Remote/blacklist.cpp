#include <stdexcept>
#include <ws-gw.h>

#include <blacklist.h>
#include <log.h>

#include "flatbuffers/flatbuffers.h"
#include "global.h"

#include "proto/blacklist_generated.h"

void InitBlacklistHook() {
  state->srv.RegisterHandler("blacklist", [](WsGw::BufferView const &view) -> WsGw::Buffer {
    using namespace Mod::proto::blacklist;
    using namespace Mod;
    auto &blacklist = Blacklist::GetInstance();
    flatbuffers::Verifier verifier{view.data(), view.size()};
    auto opv = GetBlacklistOp(view.data());
    if (!opv->Verify(verifier)) throw std::runtime_error{"Failed to parse payload"};
    BlacklistOpT ops;
    opv->UnPackTo(&ops);
    for (auto op : ops.ops) {
      if (auto it = op.AsAddXUID(); it) {
        blacklist.Add(Blacklist::XUID{it->value, it->name}, it->reason, it->op);
      } else if (auto it = op.AsAddUUID(); it) {
        blacklist.Add(Blacklist::UUID{it->value, it->name}, it->reason, it->op);
      } else if (auto it = op.AsAddNAME(); it) {
        blacklist.Add(Blacklist::NAME{it->value}, it->reason, it->op);
      } else if (auto it = op.AsRemoveXUID(); it) {
        blacklist.Remove(Blacklist::XUID{it->value});
      } else if (auto it = op.AsRemoveUUID(); it) {
        blacklist.Remove(Blacklist::UUID{it->value});
      } else if (auto it = op.AsRemoveNAME(); it) {
        blacklist.Remove(Blacklist::NAME{it->value});
      } else {
        throw std::runtime_error{"Unknown or null entry"};
      }
    }
    return {};
  });
}