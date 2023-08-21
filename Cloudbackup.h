#pragma once
#include "aws/transfer/TransferManager.h"
#include <memory>
namespace Aws
{
  namespace Utils
  {
    //class 
  }
}
namespace s3upload
{
  class CloudBackup
  {
  public:
    CloudBackup(const std::map<std::string, std::string>& aConfig);
    ~CloudBackup();
  private:
    std::unique_ptr<Aws::Utils::Threading::PooledThreadExecutor> mExecutor;
    std::unique_ptr<Aws::S3::S3Client> mS3Client;
    std::shared_ptr<Aws::Transfer::TransferManager> mTransfer;
  };

}