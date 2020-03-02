#pragma once

#include <memory>

#include "LevelStorage.h"

class DBStorage : public LevelStorage {
public:
  __declspec(dllimport) virtual ~DBStorage();
  __declspec(dllimport) virtual void addStorageObserver(std::unique_ptr<LevelStorageObserver> observer);
  __declspec(dllimport) virtual void tick();
  __declspec(dllimport) virtual std::unique_ptr<CompoundTag> getCompoundTag(std::string const &key);
  __declspec(dllimport) virtual bool hasKey(gsl::cstring_span name) const;
  __declspec(dllimport) virtual void forEachKeyWithPrefix(
      gsl::cstring_span prefix, std::function<void(gsl::cstring_span, gsl::cstring_span)>) const;
  __declspec(dllimport) virtual bool loadLevelData(LevelData &);
  __declspec(dllimport) virtual std::unique_ptr<ChunkSource> createChunkStorage(
      std::unique_ptr<ChunkSource>, StorageVersion version);
  __declspec(dllimport) virtual void saveLevelData(LevelData const &);
  __declspec(dllimport) virtual Core::PathBuffer<std::string> const &getFullPath() const;
  __declspec(dllimport) virtual std::shared_ptr<Bedrock::Threading::IAsyncResult<void>> saveData(
      std::string const &key, std::string &&value);
  __declspec(dllimport) virtual std::unique_ptr<Batch> createWriteBatch();
  __declspec(dllimport) virtual void saveDataSync(gsl::cstring_span key, gsl::cstring_span value);
  __declspec(dllimport) virtual std::shared_ptr<Bedrock::Threading::IAsyncResult<void>> deleteData(
      std::string const &key);
  __declspec(dllimport) virtual void syncIO();
  __declspec(dllimport) virtual void getStatistics(std::string &stat);
  // clonePlayerData
  __declspec(dllimport) virtual bool isCorrupted() const;
  __declspec(dllimport) virtual void startShutdown();
  __declspec(dllimport) virtual bool checkShutdownDone();
  __declspec(dllimport) virtual bool loadData(gsl::cstring_span source, std::string &buffer) const;
  __declspec(dllimport) virtual Core::LevelStorageResult getState() const;
  __declspec(dllimport) virtual std::vector<SnapshotFilenameAndLength> createSnapshot(std::string const &key);
  __declspec(dllimport) virtual void releaseSnapshot();
  __declspec(dllimport) virtual void compactStorage();
  __declspec(dllimport) virtual void syncAndSuspendStorage();
  __declspec(dllimport) virtual void resumeStorage();
  __declspec(dllimport) virtual void setFlushAllowed(bool);
  __declspec(dllimport) virtual void flushToPermanentStorage();
  __declspec(dllimport) virtual void freeCaches();
  __declspec(dllimport) virtual void setCompactionCallback(std::function<void(CompactionStatus)>);
  __declspec(dllimport) virtual void setCriticalSyncSaveCallback(std::function<void()>);
  __declspec(dllimport) virtual void corruptLevel();
  __declspec(dllimport) virtual void _saveDataSync(gsl::cstring_span key, std::string &&value);
};