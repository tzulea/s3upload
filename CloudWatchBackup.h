#pragma once
#include "CloudBackup.h"
#include <deque>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>


namespace s3upload
{
  class IWatcher; //fwd

  class CloudWatchBackup : public CloudBackup
  {
  public:
    CloudWatchBackup(const std::map<std::string, std::string>& aConfig);
    ~CloudWatchBackup();
    void onNewFileAdded(const std::filesystem::path& aPath);
    void watch(const std::filesystem::path& aPath);
  private:

    std::deque<std::filesystem::path> mUploadQueue;
    std::thread mUploadThread;
    std::mutex mUploadMutex;
    std::condition_variable mCv;
    std::atomic_bool mStop;
    std::unique_ptr<IWatcher> mFsWatcher;
  };
}