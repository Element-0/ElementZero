#include "ChakraCommon.h"
#include "mods/ScriptSupport/chakra_helper.h"
#include <cstring>
#include <sstream>
#include <stdexcept>

#include <mods/ScriptSupport/scriptapi.h>

using namespace Mod::Scripting;

namespace {
struct TextEncoder {
  static JsValueRef encode(std::string inp) {
    JsValueRef out;
    ThrowError(JsCreateTypedArray(JsTypedArrayType::JsArrayTypeUint8, JS_INVALID_REFERENCE, 0, inp.size(), &out));
    ChakraBytePtr buffer;
    unsigned int length;
    ThrowError(JsGetTypedArrayStorage(out, &buffer, &length, NULL, NULL));
    std::memcpy(buffer, inp.data(), length);
    return out;
  }
  static JsValueRef InitProto() {
    static LeakedHolder proto = IIFE([] {
      JsObjectWrapper Proto;
      Proto["encode"] = &TextEncoder::encode;
      return *Proto;
    });
    return *proto;
  }
};
struct TextDecoder {
  static JsValueRef decode(JsValueRef inp) {
    ChakraBytePtr buffer;
    unsigned int length;
    switch (GetJsType(inp)) {
    case JsTypedArray: {
      ThrowError(JsGetTypedArrayStorage(inp, &buffer, &length, NULL, NULL));
    } break;
    case JsArrayBuffer: {
      JsGetArrayBufferStorage(inp, &buffer, &length);
    } break;
    default:
      throw JsWrapError(
          JsCreateTypeError,
          "Failed to execute 'decode' on 'TextDecoder': The provided value is not of type '(ArrayBuffer or "
          "ArrayBufferView)");
    }
    JsValueRef ref;
    ThrowError(JsCreateString((char const *) buffer, length, &ref));
    return ref;
  }
  static JsValueRef InitProto() {
    static LeakedHolder proto = IIFE([] {
      JsObjectWrapper Proto;
      Proto["decode"] = &TextDecoder::decode;
      return *Proto;
    });
    return *proto;
  }
};

}; // namespace

static RegisterQueue queue("text", [](JsObjectWrapper global) {
  global["TextEncoder"] = [](JsValueRef callee, Arguments args) -> JsValueRef {
    if (!args.isConstructCall)
      throw JsWrapError(JsCreateTypeError, "Failed to construct 'TextEncoder': Please use the 'new' operator");
    JsSetPrototype(callee, TextEncoder::InitProto());
    return callee;
  };
  global["TextEncoder"].get<JsObjectWrapper>()["prototype"] = TextEncoder::InitProto();

  global["TextDecoder"] = [](JsValueRef callee, Arguments args) -> JsValueRef {
    if (!args.isConstructCall)
      throw JsWrapError(JsCreateTypeError, "Failed to construct 'TextDecoder': Please use the 'new' operator");
    JsSetPrototype(callee, TextDecoder::InitProto());
    return callee;
  };
  global["TextDecoder"].get<JsObjectWrapper>()["prototype"] = TextDecoder::InitProto();
});