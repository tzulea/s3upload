#include "WatcherLinux.h"
#include <sys/inotify.h>
#include <unistd.h>

namespace s3upload
{
  WatcherLinux::WatcherLinux()
    :mStop(false),
    mWatchPath(),
    mInotify(0)
  {
  }

  WatcherLinux::~WatcherLinux()
  {
    mStop = true;

    auto it = mWatches.begin();
    while (it != mWatches.end())
    {
      inotify_rm_watch(mInotify, it->second);
      it = mWatches.erase(it);
    }
    close(mInotify);
    mWatcher.join();
  }

  void WatcherLinux::watch(const std::filesystem::path& aPath)
  {
    mWatchPath = aPath;
    mInotify = inotify_init();
    addWatch(aPath);
    auto dirIterator = std::filesystem::recursive_directory_iterator(aPath);
    for (const auto& it : dirIterator)
    {
      if (it.is_directory())
      {
        addWatch(it.path());
      }
    }
    mWatcher = std::thread([&] 
      {
        constexpr int buffLength = 1024 * 1024; // 1MB
        while (!mStop)
        {
          auto buffer = std::make_unique<char[]>(buffLength);
          int length = read(mInotify, buffer.get(), buffLength);
          if (length < 0 || mStop)
          {
            return;
          }
          handleEvents(std::move(buffer), length);
        }
      
      });
  }

  void WatcherLinux::addWatch(const std::filesystem::path& aPath)
  {
    auto wd = inotify_add_watch(mInotify, mWatchPath.c_str(), IN_CREATE | IN_DELETE_SELF | IN_MOVED_TO);
    mWatches.emplace(aPath, wd);
  }

  void WatcherLinux::handleEvents(std::unique_ptr<char[]>&& aBuffer, int length)
  {
    int i = 0;
    while (i < length)
    {
      struct inotify_event* event = (struct inotify_event*)&aBuffer[i];
      if (event->len)
      {
        std::filesystem::path fullPath = mWatchPath;
        fullPath.append(event->name);
        if (event->mask & IN_DELETE_SELF)
        {

          auto it = mWatches.find(fullPath);
          if (it != mWatches.end())
          {
            auto wd = it->second;
            inotify_rm_watch(mInotify, wd);
            mWatches.erase(it);
          }
          i += (sizeof(inotify_event) + event->len);
          continue;
        }
        if (event->mask & IN_ISDIR)
        {
          addWatch(fullPath);
        }
        if (onNewFile)
        {
          onNewFile(fullPath);
        }
       i += (sizeof(inotify_event) + event->len);
      }
    }
  }
}