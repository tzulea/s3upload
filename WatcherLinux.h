#pragma once
#include "IWatcher.h"
#include <atomic>
#include <memory>
#include <thread>
#include <map>

namespace s3upload
{
  class WatcherLinux : public IWatcher
  {
  public:
    WatcherLinux();
    ~WatcherLinux();
    void watch(const std::filesystem::path& aPath) override final;

  private:
    void handleEvents(std::unique_ptr<char[]>&& aBuffer, int length);
    void addWatch(const std::filesystem::path& aPath);
    std::atomic_bool mStop;
    std::filesystem::path mWatchPath;
    std::thread mWatcher;
    int mInotify;
    std::map<std::filesystem::path, int> mWatches;
  };
}