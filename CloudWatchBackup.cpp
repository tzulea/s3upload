#include "CloudWatchBackup.h"
#ifdef _WIN32
#include "WatcherWin.h"
#endif // _WIN32

namespace s3upload
{
  CloudWatchBackup::CloudWatchBackup(const std::map<std::string, std::string>& aConfig)
    :CloudBackup(aConfig),
     mStop(false)
  {

  }
  CloudWatchBackup::~CloudWatchBackup()
  {
    mStop = true;
    mCv.notify_one();
    mFsWatcher = nullptr;
    mUploadThread.join();
  }
  void CloudWatchBackup::onNewFileAdded(const std::filesystem::path& aPath)
  {
    {
      std::lock_guard<std::mutex> lock(mUploadMutex);
      mUploadQueue.emplace_back(aPath);
    }
    mCv.notify_one();
  }

  void CloudWatchBackup::watch(const std::filesystem::path& aPath)
  {
#ifdef _WIN32
    mFsWatcher = std::make_unique<WatcherWin>();
#endif // _WIN32
    mFsWatcher->onNewFile = std::bind(&CloudWatchBackup::onNewFileAdded, this, std::placeholders::_1);
    //start our thread
    mUploadThread = std::thread([&]
      {
        while (!mStop)
        {
          std::unique_lock<std::mutex> lock(mUploadMutex);
          mCv.wait(lock, [this] { return !mUploadQueue.empty() || mStop; });
          if (mStop)
          {
            return;
          }
          auto newItem = mUploadQueue.front();
          mUploadQueue.pop_front();
          upload(newItem.string());
        }
      });
    mFsWatcher->watch(aPath);
  }
}