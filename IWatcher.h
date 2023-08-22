#pragma once
#include <string>
#include <functional>
#include <filesystem>
namespace s3upload
{
  class IWatcher
  {
  public:
    std::function<void(const std::filesystem::path& aFilePath)> onNewFile;
    virtual void watch(const std::filesystem::path& aPath) = 0;
  };
}