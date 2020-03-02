#pragma once

#include <system_error>
#include <exception>
#include <functional>

#include "AsyncBase.h"
#include "AsyncStatus.h"

namespace Bedrock::Threading {

template <typename T = void> class IAsyncResult {
  virtual ~IAsyncResult() {}
  virtual AsyncStatus getStatus() const                                    = 0;
  virtual std::error_code getError() const                                 = 0;
  virtual std::exception_ptr getException() const                          = 0;
  virtual void cancel()                                                    = 0;
  virtual void addOnComplete(std::function<IAsyncResult<void> const &> cb) = 0;
};

namespace AsyncResult {
__declspec(dllimport) std::shared_ptr<IAsyncResult<void>> done();

template <typename T = void> class CompleteResult : public IAsyncResult<T> {
  virtual AsyncStatus getStatus() const { return AsyncStatus::Complete; }
  virtual std::error_code getError() const { return {}; }
  virtual std::exception_ptr getException() const { return {}; }
  virtual void cancel(){};
  virtual void addOnComplete(std::function<IAsyncResult<T> const &> cb) {
    if (fn) fn(*this);
  }
};
}; // namespace AsyncResult

} // namespace Bedrock::Threading