#include "WatcherWin.h"
#include <thread>

namespace s3upload
{
  WatcherWin::WatcherWin()
    :mStop(false),
    mOverLapped(),
    mWatchPath()
  {
  }

  WatcherWin::~WatcherWin()
  {
    mStop = true;
    SetEvent(mOverLapped.hEvent);
    mWatcherWin.join();
  }

  void WatcherWin::watch(const std::filesystem::path& aPath)
  {
    mWatchPath = aPath;

    mWatcherWin = std::thread([&] {
      constexpr int filter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE;
      constexpr int length = 1024 * 1024; // 1MB
      auto buffer = std::make_unique<std::uint8_t[]>(length);
      mOverLapped.hEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
      auto fileHandle = CreateFileW(mWatchPath.wstring().c_str(), GENERIC_READ | FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
      if (fileHandle == INVALID_HANDLE_VALUE)
      {
        return;
      }
      while (!mStop)
      {
        auto result = ReadDirectoryChangesW(fileHandle, buffer.get(), length, true, filter, nullptr, &mOverLapped, nullptr);
        if (!result)
        {
          auto err = GetLastError();
          break;
        }
        auto waitResult = WaitForSingleObject(mOverLapped.hEvent, INFINITE);
        if (mStop)
        {
          break;
        }
        if (waitResult != WAIT_OBJECT_0)
        {
          break;
        }
        handleEvents(std::move(buffer));
        buffer = std::make_unique<std::uint8_t[]>(length); // new buffer for next cycle
      }
      // cleanup
      CloseHandle(mOverLapped.hEvent);
      CloseHandle(fileHandle);
      }
    ); // end thread function
  }
  void WatcherWin::handleEvents(std::unique_ptr<std::uint8_t[]>&& aBuffer)
  {
    auto iter = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(aBuffer.get());
    while (iter)
    {
      if (iter->Action == FILE_ACTION_ADDED)
      {
        std::wstring fileNamePath = std::wstring(iter->FileName, iter->FileNameLength);
        std::filesystem::path fileAdded = mWatchPath;
        fileAdded.append(fileNamePath);
        if (onNewFile)
        {
          onNewFile(fileAdded);
        }
      }
      iter = iter + iter->NextEntryOffset;
    }
  }
}