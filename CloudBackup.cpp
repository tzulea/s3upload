#include "Cloudbackup.h"
#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/Aws.h>
#include "constants.h"
namespace s3upload
{
  CloudBackup::CloudBackup(const std::map<std::string, std::string>& aConfig)
  {
    Aws::SDKOptions options;
    Aws::InitAPI(options);

    auto s3Cred = Aws::Auth::AWSCredentials();
    s3Cred.SetAWSAccessKeyId(aConfig.at(awsCred));
    s3Cred.SetAWSSecretKey(aConfig.at(awsSecret));
    mS3Client = std::make_unique<Aws::S3::S3Client>(s3Cred);

    auto nThreads = std::thread::hardware_concurrency();
    mExecutor = std::make_unique<Aws::Utils::Threading::PooledThreadExecutor>(nThreads);

    Aws::Transfer::TransferManagerConfiguration config(mExecutor.get());
    mTransfer = Aws::Transfer::TransferManager::Create(config);
  }
  CloudBackup::~CloudBackup()
  {
    Aws::SDKOptions options;
    Aws::ShutdownAPI(options);
  }
}