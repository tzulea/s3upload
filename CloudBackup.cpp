#include "Cloudbackup.h"
#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/Aws.h>
#include "constants.h"
#include <filesystem>

namespace s3upload
{
  CloudBackup::CloudBackup(const std::map<std::string, std::string>& aConfig)
  {
    Aws::SDKOptions options;
    Aws::InitAPI(options);

    auto s3Cred = Aws::Auth::AWSCredentials();
    s3Cred.SetAWSAccessKeyId(aConfig.at(awsCred));
    s3Cred.SetAWSSecretKey(aConfig.at(awsSecret));
    mS3Client = std::make_shared<Aws::S3::S3Client>(s3Cred);

    auto nThreads = std::thread::hardware_concurrency();
    mExecutor = std::make_unique<Aws::Utils::Threading::PooledThreadExecutor>(nThreads);

    Aws::Transfer::TransferManagerConfiguration config(mExecutor.get());
    config.s3Client = mS3Client;
    config.errorCallback = std::bind(&CloudBackup::onUploadError, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    config.uploadProgressCallback = std::bind(&CloudBackup::onUploadProgress, this, std::placeholders::_1, std::placeholders::_2);
    config.transferInitiatedCallback = std::bind(&CloudBackup::onTransferInit, this, std::placeholders::_1, std::placeholders::_2);
    config.transferStatusUpdatedCallback = std::bind(&CloudBackup::onTransferStatusUpdate, this, std::placeholders::_1, std::placeholders::_2);
    mTransfer = Aws::Transfer::TransferManager::Create(config);
    mConfig = aConfig;
  }

  CloudBackup::~CloudBackup()
  {
    Aws::SDKOptions options;
    Aws::ShutdownAPI(options);
  }
  void CloudBackup::onUploadProgress(const Aws::Transfer::TransferManager* aManager, const std::shared_ptr<const Aws::Transfer::TransferHandle>& aHandle)
  {
  }

  void CloudBackup::onUploadError(const Aws::Transfer::TransferManager* aManager, const std::shared_ptr<const Aws::Transfer::TransferHandle>& aHandle, const Aws::Client::AWSError<Aws::S3::S3Errors>& aError)
  {
    auto x = aError.GetMessage();
  }

  void CloudBackup::onTransferInit(const Aws::Transfer::TransferManager* amanager, const std::shared_ptr<const Aws::Transfer::TransferHandle>& aHandle)
  {
  }

  void CloudBackup::onTransferStatusUpdate(const Aws::Transfer::TransferManager* amanager, const std::shared_ptr<const Aws::Transfer::TransferHandle>& aHandle)
  {

  }

  void CloudBackup::upload(const std::string& aPath)
  {
    bool isDir = std::filesystem::is_directory(aPath);
    auto bucket = mConfig[bucketName];
    std::string filename;
    if (!mConfig[bucketFolder].empty())
    {
      filename = mConfig[bucketFolder];
      filename.append("/");
    }
    auto name = std::filesystem::path(aPath).filename().string();
    filename.append(name);
    if (!isDir)
    {
      mTransfer->UploadFile(aPath, bucket, filename, "application/octet-stream", Aws::Map<Aws::String, Aws::String>());
    }
    else
    {
      mTransfer->UploadDirectory(aPath, bucket, filename, Aws::Map<Aws::String, Aws::String>());
    }
    mTransfer->WaitUntilAllFinished();
  }


}