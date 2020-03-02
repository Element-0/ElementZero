#pragma once

#include <memory>
#include <functional>
#include <boost/format.hpp>
#include <vector>

#include <gsl-lite.hpp>

#include <Core/core.h>
#include <Bedrock/Threading/AsyncResult.h>

class LevelStorageObserver;
class CompoundTag;
class LevelData;
class ChunkSource;
enum class StorageVersion;

struct SnapshotFilenameAndLength {
  std::string filename;
  std::size_t length;

  inline std::string toString() const { return (boost::format("%s:%d") % filename % length).str(); }
};

enum class CompactionStatus {};

class LevelStorage {
public:
  class Batch {};
  __declspec(dllimport) virtual ~LevelStorage();
  virtual void addStorageObserver(std::unique_ptr<LevelStorageObserver> observer) = 0;
  virtual void tick()                                                             = 0;
  virtual std::unique_ptr<CompoundTag> getCompoundTag(std::string const &key)     = 0;
  virtual bool hasKey(gsl::cstring_span key) const                                = 0;
  virtual void
  forEachKeyWithPrefix(gsl::cstring_span prefix, std::function<void(gsl::cstring_span, gsl::cstring_span)>) const = 0;
  virtual bool loadLevelData(LevelData &)                                                                         = 0;
  virtual std::unique_ptr<ChunkSource> createChunkStorage(std::unique_ptr<ChunkSource>, StorageVersion version)   = 0;
  virtual void saveLevelData(LevelData const &)                                                                   = 0;
  virtual Core::PathBuffer<std::string> const &getFullPath() const                                                = 0;
  virtual std::shared_ptr<Bedrock::Threading::IAsyncResult<void>>
  saveData(std::string const &key, std::string &&value) = 0;
  __declspec(dllimport) virtual std::unique_ptr<Batch> createWriteBatch();
  virtual void saveDataSync(gsl::cstring_span key, gsl::cstring_span value)                          = 0;
  virtual std::shared_ptr<Bedrock::Threading::IAsyncResult<void>> deleteData(std::string const &key) = 0;
  virtual void syncIO()                                                                              = 0;
  virtual void getStatistics(std::string &stat)                                                      = 0;
  __declspec(dllimport) virtual bool clonePlayerData(gsl::cstring_span source, gsl::cstring_span target);
  virtual bool isCorrupted() const                                                                 = 0;
  virtual void startShutdown()                                                                     = 0;
  virtual bool checkShutdownDone()                                                                 = 0;
  __declspec(dllimport) virtual bool loadData(gsl::cstring_span source, std::string &buffer) const = 0;
  virtual Core::LevelStorageResult getState() const                                                = 0;
  virtual std::vector<SnapshotFilenameAndLength> createSnapshot(std::string const &key)            = 0;
  virtual void releaseSnapshot()                                                                   = 0;
  virtual void compactStorage()                                                                    = 0;
  virtual void syncAndSuspendStorage()                                                             = 0;
  virtual void resumeStorage()                                                                     = 0;
  virtual void setFlushAllowed(bool)                                                               = 0;
  virtual void flushToPermanentStorage()                                                           = 0;
  __declspec(dllimport) virtual void freeCaches();
  virtual void setCompactionCallback(std::function<void(CompactionStatus)>) = 0;
  virtual void setCriticalSyncSaveCallback(std::function<void()>)           = 0;
  __declspec(dllimport) virtual void corruptLevel();
  virtual void _saveDataSync(gsl::cstring_span key, std::string &&value) = 0;
};