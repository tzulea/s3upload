#pragma once
#include "aws/transfer/TransferManager.h"
#include <memory>

namespace s3upload
{
  class CloudBackup
  {
  public:
    CloudBackup(const std::map<std::string, std::string>& aConfig);
    ~CloudBackup();
  private:
    void onUploadProgress(const Aws::Transfer::TransferManager* aManager, const std::shared_ptr<const Aws::Transfer::TransferHandle>& aHandle);
    void onUploadError(const Aws::Transfer::TransferManager* aManager, const std::shared_ptr<const Aws::Transfer::TransferHandle>& aHandle, const Aws::Client::AWSError<Aws::S3::S3Errors>& aError);

    std::unique_ptr<Aws::Utils::Threading::PooledThreadExecutor> mExecutor;
    std::shared_ptr<Aws::S3::S3Client> mS3Client;
    std::shared_ptr<Aws::Transfer::TransferManager> mTransfer;


  };

}