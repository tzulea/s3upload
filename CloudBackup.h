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
    void upload(const std::string& aPath);
  private:
    void onUploadProgress(const Aws::Transfer::TransferManager* aManager, const std::shared_ptr<const Aws::Transfer::TransferHandle>& aHandle);
    void onUploadError(const Aws::Transfer::TransferManager* aManager, const std::shared_ptr<const Aws::Transfer::TransferHandle>& aHandle, const Aws::Client::AWSError<Aws::S3::S3Errors>& aError);
    void onTransferInit(const Aws::Transfer::TransferManager* amanager, const std::shared_ptr<const Aws::Transfer::TransferHandle>& aHandle);
    void onTransferStatusUpdate(const Aws::Transfer::TransferManager* amanager, const std::shared_ptr<const Aws::Transfer::TransferHandle>& aHandle);

    std::unique_ptr<Aws::Utils::Threading::PooledThreadExecutor> mExecutor;
    std::shared_ptr<Aws::S3::S3Client> mS3Client;
    std::shared_ptr<Aws::Transfer::TransferManager> mTransfer;
    std::map<std::string, std::string> mConfig;


  };

}
