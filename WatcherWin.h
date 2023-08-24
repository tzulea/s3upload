#pragma once
#include "IWatcher.h"
#include <Windows.h>
#include <atomic>
#include <memory>
#include <thread>

namespace s3upload
{
  class WatcherWin : public IWatcher
  {
  public:
    WatcherWin();
    ~WatcherWin();
    void watch(const std::filesystem::path& aPath) override final;

  private:
    void handleEvents(std::unique_ptr<std::uint8_t[]>&& aBuffer);
    OVERLAPPED mOverLapped;
    std::atomic_bool mStop;
    std::filesystem::path mWatchPath;
    std::thread mWatcherWin;

  };
}