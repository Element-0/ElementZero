#include <iomanip>
#include <string>
#include <memory>
#include <mutex>

#include <SQLiteCpp/SQLiteCpp.h>

#include <hook.h>
#include <log.h>

#include "settings.hpp"

enum class LogArea {};
enum class LogAreaID : unsigned char {
  ALL,
  PLATFORM,
  ENTITY,
  DATABASE,
  GUI,
  SYSTEM,
  NETWORK,
  RENDER,
  MEMORY,
  ANIMATION,
  INPUT,
  LEVEL,
  SERVER,
  DLC,
  PHYSICS,
  FILE,
  STORAGE,
  REALMS,
  REALMSAPI,
  XBOXLIVE,
  USERMANAGER,
  XSAPI,
  PERF,
  TELEMETRY,
  BLOCKS,
  RAKNET,
  GAMEFACE,
  SOUND,
  INTERACTIVE,
  SCRIPTING,
  PLAYFAB,
  AUTOMATION,
  PERSONA,
  MODDING
};

class BedrockLog {
public:
  class LogDetails {
    __declspec(dllimport) void _logToFile(std::string const &);

  public:
    void proxy(std::string const &data) { _logToFile(data); }
  };

  __declspec(dllexport) static char const *_areaFilterString(LogAreaID id) {
    switch (id) {
    case LogAreaID::ALL: return "ALL";
    case LogAreaID::PLATFORM: return "PLATFORM";
    case LogAreaID::ENTITY: return "ENTITY";
    case LogAreaID::DATABASE: return "DATABASE";
    case LogAreaID::GUI: return "GUI";
    case LogAreaID::SYSTEM: return "SYSTEM";
    case LogAreaID::NETWORK: return "NETWORK";
    case LogAreaID::RENDER: return "RENDER";
    case LogAreaID::MEMORY: return "MEMORY";
    case LogAreaID::ANIMATION: return "ANIMATION";
    case LogAreaID::INPUT: return "INPUT";
    case LogAreaID::LEVEL: return "LEVEL";
    case LogAreaID::SERVER: return "SERVER";
    case LogAreaID::DLC: return "DLC";
    case LogAreaID::PHYSICS: return "PHYSICS";
    case LogAreaID::FILE: return "FILE";
    case LogAreaID::STORAGE: return "STORAGE";
    case LogAreaID::REALMS: return "REALMS";
    case LogAreaID::REALMSAPI: return "REALMSAPI";
    case LogAreaID::XBOXLIVE: return "XBOXLIVE";
    case LogAreaID::USERMANAGER: return "USERMANAGER";
    case LogAreaID::XSAPI: return "XSAPI";
    case LogAreaID::PERF: return "PERF";
    case LogAreaID::TELEMETRY: return "TELEMETRY";
    case LogAreaID::BLOCKS: return "BLOCKS";
    case LogAreaID::RAKNET: return "RAKNET";
    case LogAreaID::GAMEFACE: return "GAMEFACE";
    case LogAreaID::SOUND: return "SOUND";
    case LogAreaID::INTERACTIVE: return "INTERACTIVE";
    case LogAreaID::SCRIPTING: return "SCRIPTING";
    case LogAreaID::PLAYFAB: return "PLAYFAB";
    case LogAreaID::AUTOMATION: return "AUTOMATION";
    case LogAreaID::PERSONA: return "PERSONA";
    case LogAreaID::MODDING: return "MODDING";
    default: return "UNKNOWN";
    }
  }

  __declspec(dllexport) static char const *_priorityFilterString(unsigned int id) {
    switch (id) {
    case 0xFFFFFFFF: return "ALL";
    case 1: return "VERBOSE";
    case 2: return "INFO";
    case 4: return "WARN";
    case 8: return "ERROR";
    default: return "UNKNOWN";
    }
  }
};

class ContentLog {
public:
  __declspec(dllimport) static char const *getLogAreaName(LogArea);
};

static char getPriority(unsigned int pri) {
  switch (pri) {
  case 1: return 'V';
  case 2: return 'I';
  case 4: return 'W';
  case 8: return 'E';
  default: return 'U';
  }
}
static int getPriorityDecoration(unsigned int pri) {
  switch (pri) {
  case 1: return 0;
  case 2: return 1;
  case 4: return 2;
  case 8: return 3;
  default: return 4;
  }
}

static std::unique_ptr<SQLite::Database> log_database;
static BedrockLog::LogDetails *log_instance;

void initDatabase() {
  DEF_LOGGER("LogDatabase");
  if (settings.LogSettings.Database != "") {
    log_database =
        std::make_unique<SQLite::Database>(settings.LogSettings.Database, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    log_database->exec(
        "CREATE TABLE IF NOT EXISTS log ("
        "session TEXT, "
        "time NUMERIC, "
        "priority INTEGER, "
        "area TEXT, "
        "source TEXT, "
        "line INTEGER, "
        "content TEXT)");
    log_database->exec(
        "CREATE TEMP TABLE pending ("
        "time NUMERIC, "
        "priority INTEGER, "
        "area TEXT, "
        "source TEXT, "
        "line INTEGER, "
        "content TEXT)");
    // You don't need data safety for log
    log_database->exec("PRAGMA journal_mode = WAL");
    log_database->exec("PRAGMA synchronous = NORMAL");
    LOGV("initialized (location %s)") % settings.LogSettings.Database;
  }
}

TInstanceHook(void, "?_openLogFile@LogDetails@BedrockLog@@AEAAXXZ", BedrockLog::LogDetails) {
  original(this);
  log_instance = this;
}

std::set<std::string_view> LogFilters;

void generalLog(unsigned int pri, std::string_view area, char const *source, unsigned line, std::string content) {
  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  auto f          = boost::format(settings.LogSettings.Format) % getPriority(pri) % area % source % line % content %
           std::put_time(std::localtime(&now), "%Y-%m-%d %X");
  auto data = f.str();
  if (data.back() != '\n') data.append("\n");
  auto deco = settings.LogSettings.Decorations[getPriorityDecoration(pri)];
  if (!LogFilters.count(area) && (!settings.LogSettings.HideVerbose || pri > 1))
    std::cout << deco.Before << data << deco.After << std::flush;
  if (log_instance) log_instance->proxy(data);
  if (log_database) {
    static std::once_flag of;
    std::call_once(
        of, [] { LogFilters.insert(settings.LogSettings.Filters.begin(), settings.LogSettings.Filters.end()); });
    try {
      static SQLite::Statement insert{
          *log_database,
          "INSERT INTO log (session, time, priority, area, source, line, content) "
          "VALUES (?, CURRENT_TIMESTAMP, ?, ?, ?, ?, ?)"};
      if (session != "") {
        static std::once_flag o;
        std::call_once(o, [] {
          SQLite::Statement stmt{
              *log_database,
              "INSERT INTO log SELECT "
              "?, time, priority, area, source, line, content "
              "FROM temp.pending"};
          stmt.bind(1, session);
          stmt.exec();
        });
        insert.bindNoCopy(1, session);
        insert.bind(2, pri);
        insert.bindNoCopy(3, area.data());
        insert.bindNoCopy(4, source);
        insert.bind(5, line);
        insert.bindNoCopy(6, content);
        insert.exec();
        insert.reset();
        insert.clearBindings();
      } else {
        static SQLite::Statement insert_temp{
            *log_database,
            "INSERT INTO temp.pending (time, priority, area, source, line, content) "
            "VALUES (CURRENT_TIMESTAMP, ?, ?, ?, ?, ?)"};
        insert_temp.bind(1, pri);
        insert_temp.bindNoCopy(2, area.data());
        insert_temp.bindNoCopy(3, source);
        insert_temp.bind(4, line);
        insert_temp.bindNoCopy(5, data);
        insert_temp.exec();
        insert_temp.reset();
        insert_temp.clearBindings();
      }
    } catch (std::exception &ex) {
      log_database.reset();
      std::cerr << ex.what() << std::endl;
    }
  }
}

void Mod::Logger::commit(Level level, unsigned line, std::string value) {
  generalLog((unsigned) level, name, source, line, value);
}

TInstanceHook(
    void, "?_log_va@LogDetails@BedrockLog@@AEAAXW4LogAreaID@@IPEBDHH1PEAD@Z", BedrockLog::LogDetails, LogAreaID id,
    unsigned pri, char const *source, int line, int index, char const *format, va_list vargs) {
  static thread_local char buffer[65536];
  vsprintf_s(buffer, format, vargs);
  generalLog(pri, BedrockLog::_areaFilterString(id), source, line, buffer);
}