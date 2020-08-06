#include <stdexcept>

#include <Core/mce.h>

#include <sqlite3.h>

#include <modutils.h>

#include <mods/ScriptSupport/scriptapi.h>

using namespace Mod::Scripting;

namespace Mod::Scripting {

struct ErrMsgStorage {
  char *errmsg;
  ~ErrMsgStorage() {
    if (errmsg) sqlite3_free(errmsg);
  }

  inline std::string build(std::string inp) { return inp + errmsg; }

  operator char * *() { return &errmsg; }
};

template <int sig = SQLITE_OK> struct SqliteErrorCode {
  std::string ctxmsg;

  SqliteErrorCode(std::string ctxmsg) : ctxmsg(ctxmsg) {}

  void operator=(int value) {
    if (value != sig) {
      auto msg = sqlite3_errstr(value);
      auto f   = boost::format("%s (%d) %s") % ctxmsg % value % msg;
      throw std::runtime_error{f.str()};
    }
  }
};

struct Stmt {
  sqlite3_stmt *stmt;
  std::map<std::string, int> cache;

  Stmt(sqlite3_stmt *stmt) : stmt(stmt) {}

  void Reset() { sqlite3_reset(stmt); }

  void ClearBindings() { sqlite3_clear_bindings(stmt); }

  void Bind_(int key, JsValueRef value) {
    SqliteErrorCode ec{"Failed to bind value: "};
    switch (GetJsType(value)) {
    case JsNumber: ec = sqlite3_bind_double(stmt, key, FromJs<double>(value)); break;
    case JsBoolean: ec = sqlite3_bind_int(stmt, key, FromJs<bool>(value)); break;
    case JsNull:
    case JsUndefined: ec = sqlite3_bind_null(stmt, key); break;
    case JsString: {
      auto temp = FromJs<std::string>(value);
      ec        = sqlite3_bind_text(stmt, key, temp.c_str(), temp.size(), nullptr);
      break;
    }
    case JsTypedArray: {
      ChakraBytePtr buffer;
      unsigned int len;
      ThrowError(JsGetTypedArrayStorage(value, &buffer, &len, nullptr, nullptr));
      ec = sqlite3_bind_blob64(stmt, key, buffer, len, nullptr);
      break;
    }
    case JsArrayBuffer: {
      ChakraBytePtr buffer;
      unsigned int len;
      ThrowError(JsGetArrayBufferStorage(value, &buffer, &len));
      ec = sqlite3_bind_blob64(stmt, key, buffer, len, nullptr);
      break;
    }
    default: throw std::runtime_error{"Illegal type in bind"};
    }
  }

  void Bind_(std::string const &key, JsValueRef value) {
    if (auto it = cache.find(key); it != cache.end()) return Bind_(it->second, value);
    auto idx = sqlite3_bind_parameter_index(stmt, key.c_str());
    if (idx == 0) throw std::runtime_error{"Index name not found"};
    cache[key] = idx;
    return Bind_(idx, value);
  }

  void Bind(JsValueRef key, JsValueRef value) {
    switch (GetJsType(key)) {
    case JsString: Bind_(FromJs<std::string>(key), value); break;
    case JsNumber: Bind_(FromJs<int>(key), value); break;
    default: throw std::runtime_error{"Unexcpted key type"};
    }
  }

  void BindAll(JsValueRef maps) {
    switch (GetJsType(maps)) {
    case JsArray: {
      JsValueRef xlen;
      ThrowError(JsObjectGetProperty(maps, ToJs("length"), &xlen));
      int len;
      ThrowError(JsNumberToInt(xlen, &len));
      for (int i = 0; i < len; i++) {
        JsValueRef temp;
        ThrowError(JsGetIndexedProperty(maps, ToJs(i), &temp));
        Bind_(i + 1, temp);
      }
      break;
    }
    case JsObject: {
      JsValueRef arr;
      ThrowError(JsGetOwnPropertyNames(maps, &arr));
      JsObjectWrapper arrwrap{arr};
      auto len = arrwrap["length"].get<int>();
      for (int i = 0; i < len; i++) {
        JsValueRef temp_key, temp_value;
        ThrowError(JsGetIndexedProperty(maps, ToJs(i), &temp_key));
        ThrowError(JsObjectGetProperty(maps, temp_key, &temp_value));
        Bind(temp_key, temp_value);
      }
      break;
    }
    default: throw std::runtime_error{"Unexcpted key type"};
    }
  }

  void ForEach(JsValueRef fn) {
    if (GetJsType(fn) != JsFunction) throw std::runtime_error{"Expect function"};
    int ec;
    auto cols = sqlite3_column_count(stmt);
    JsValueRef arr[cols + 1];
    arr[0] = GetUndefined();
    while ((ec = sqlite3_step(stmt)) == SQLITE_ROW) {
      for (int col = 0; col < cols; col++) {
        auto &tgt = arr[col + 1];
        switch (sqlite3_column_type(stmt, col)) {
        case SQLITE_INTEGER:
        case SQLITE_FLOAT: tgt = ToJs(sqlite3_column_double(stmt, col)); break;
        case SQLITE_NULL: tgt = GetNullValue(); break;
        case SQLITE_TEXT: tgt = ToJs((char *) sqlite3_column_text(stmt, col)); break;
        case SQLITE_BLOB: {
          auto len = sqlite3_column_bytes(stmt, col);
          ChakraBytePtr buffer;
          unsigned int _len;
          ThrowError(JsCreateArrayBuffer(len, &tgt));
          JsGetArrayBufferStorage(tgt, &buffer, &_len);
          memcpy(buffer, sqlite3_column_blob(stmt, col), len);
          break;
        }
        }
      }
      JsValueRef res;
      int jsec = JsCallFunction(fn, arr, cols + 1, &res);
      if (jsec != JsNoError) throw std::runtime_error{"Internal error"};
      if (GetJsType(res) != JsUndefined && !JsToBool(res)) break;
    }
    SqliteErrorCode<SQLITE_DONE>{"Failed to exec stmt:"} = ec;
  }

  void ForEachWith(JsValueRef ref, JsValueRef fn) {
    BindAll(ref);
    ForEach(fn);
    ClearBindings();
    Reset();
  }

  void Exec() { SqliteErrorCode<SQLITE_DONE>{"Failed to exec stmt:"} = sqlite3_step(stmt); }

  void ExecWith(JsValueRef ref) {
    BindAll(ref);
    Exec();
    ClearBindings();
    Reset();
  }

  static JsValueRef InitProto() {
    static LeakedHolder proto = IIFE([] {
      JsObjectWrapper temp;
      temp["reset"]         = &Stmt::Reset;
      temp["clearBindings"] = &Stmt::ClearBindings;
      temp["bind"]          = &Stmt::Bind;
      temp["bindAll"]       = &Stmt::BindAll;
      temp["forEach"]       = &Stmt::ForEach;
      temp["forEachWith"]   = &Stmt::ForEachWith;
      temp["exec"]          = &Stmt::Exec;
      temp["execWith"]      = &Stmt::ExecWith;
      return *temp;
    });
    return *proto;
  }

  ~Stmt() { sqlite3_finalize(stmt); }

  inline static JsObjectWrapper Create(sqlite3_stmt *stmt) {
    return JsObjectWrapper::FromExternalObject(new Stmt(stmt), InitProto());
  }
};

inline JsValueRef ToJs(sqlite3_stmt *stmt) { return *Stmt::Create(stmt); }

struct Database {
  sqlite3 *db = nullptr;

  Database(std::string const &filename) {
    SqliteErrorCode{"Failed to open database:"} =
        sqlite3_open_v2(filename.c_str(), &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr);
    SqliteErrorCode{"Failed to register function:"} = sqlite3_create_function_v2(
        db, "str2uuid", 1, true, nullptr,
        +[](sqlite3_context *ctx, int, sqlite3_value **argv) {
          auto local = mce::UUID::fromString((char const *) sqlite3_value_text(argv[0]));
          auto uuid  = new mce::UUID;
          memcpy(uuid, &local, sizeof local);
          sqlite3_result_blob(ctx, uuid, sizeof *uuid, [](void *ptr) { delete (mce::UUID *) ptr; });
        },
        nullptr, nullptr, nullptr);
    SqliteErrorCode{"Failed to register function:"} = sqlite3_create_function_v2(
        db, "uuid2str", 1, true, nullptr,
        +[](sqlite3_context *ctx, int, sqlite3_value **argv) {
          auto blob = sqlite3_value_blob(argv[0]);
          auto size = sqlite3_value_bytes(argv[0]);
          if (size == sizeof(mce::UUID)) {
            mce::UUID uuid;
            memcpy(&uuid, blob, sizeof uuid);
            auto str = uuid.asString();
            sqlite3_result_text(ctx, str.c_str(), str.length(), SQLITE_TRANSIENT);
          } else {
            sqlite3_result_error(ctx, "not a uuid", 0);
          }
        },
        nullptr, nullptr, nullptr);
  }

  ~Database() {
    if (db) sqlite3_close(db);
  }

  void Exec(std::string const &str) {
    ErrMsgStorage errmsg;
    auto ec = sqlite3_exec(db, str.c_str(), nullptr, nullptr, errmsg);
    if (ec != SQLITE_OK) throw std::runtime_error{errmsg.build("Failed to exec: ")};
  }

  sqlite3_stmt *Prepare(std::string const &str) {
    sqlite3_stmt *ret;
    SqliteErrorCode{"Failed to prepare stmt: "} = sqlite3_prepare_v2(db, str.c_str(), str.size(), &ret, nullptr);
    return ret;
  }

  static JsValueRef InitProto() {
    static LeakedHolder proto = IIFE([] {
      JsObjectWrapper temp;
      temp["exec"]    = &Database::Exec;
      temp["prepare"] = &Database::Prepare;
      return *temp;
    });
    return *proto;
  }

  inline static JsObjectWrapper Create(std::string filename) {
    return JsObjectWrapper::FromExternalObject(new Database(filename), InitProto());
  }
};

} // namespace Mod::Scripting

static ModuleRegister reg("ez:sqlite3", [](JsObjectWrapper native) -> std::string {
  native["open"] = Database::Create;
  return R"js(
    export const open = import.meta.native.open;
  )js";
});