#pragma once
#include <string>
#include <system_error>
#include <atlbase.h>
#include <wrl.h>

namespace ComHelper {

using namespace Microsoft::WRL;

struct SysHandle {
  HANDLE handle{INVALID_HANDLE_VALUE};
  char const *desc;
  SysHandle(char const *desc) : desc(desc) {}

  void operator=(HANDLE rhs) {
    if (rhs == INVALID_HANDLE_VALUE) throw std::system_error{(int) GetLastError(), std::system_category(), desc};
    handle = rhs;
  }

  ~SysHandle() {
    if (handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
  }

  operator HANDLE() const { return handle; }
};

class HR {
public:
  char const *desc;
  HR(char const *desc) : desc(desc) {}
  void operator=(HRESULT res) const {
    if (FAILED(res)) throw std::system_error{res, std::system_category(), desc};
  }
};

template <typename T> struct CoMemWrapper {
  T value{};
  operator T &() { return value; }
  operator bool() { return !!value; }
  T *operator&() { return &value; }
  ~CoMemWrapper() {
    if (!value) CoTaskMemFree(value);
  }
};

} // namespace ComHelper