#pragma once

#include <memory>

namespace Bedrock::Threading {

class AsyncBase : public std::enable_shared_from_this<AsyncBase> {
  template <typename T> std::shared_ptr<T> asShared() { return shared_from_this(); }

  ~AsyncBase() {}
};

} // namespace Bedrock::Threading