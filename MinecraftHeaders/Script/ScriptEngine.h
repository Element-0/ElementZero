#pragma once

class ScriptEngine {
public:
  struct ScriptQueueData {
    std::string relative_path, virtual_path, content, uuid, version_code;
  };
};