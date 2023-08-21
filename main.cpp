#include <cstdlib>
#include <map>
#include <string>

#include "constants.h"
#include "Cloudbackup.h"

using namespace s3upload;

std::pair<bool, std::map<std::string, std::string>> getConfig()
{
  std::map<std::string, std::string> configMap;
  auto res = std::getenv(awsCred);
  if (res == nullptr)
  {
    return std::make_pair(false, configMap);
  }
  configMap[awsCred] = res;

  res = std::getenv(awsSecret);
  if (res == nullptr)
  {
    return std::make_pair(false, configMap);
  }
  configMap[awsSecret] = res;

  res = std::getenv(bucketName);
  if (res == nullptr)
  {
    return std::make_pair(false, configMap);
  }
  configMap[bucketName] = res;

  res = std::getenv(bucketFolder);
  if (res == nullptr)
  {
    return std::make_pair(false, configMap);
  }
  configMap[bucketFolder] = res;
  return std::make_pair(true, configMap);
}

int main(int argc, char *argv[])
{
  // get config options from enviroment variable
  auto config = getConfig();

  if(!config.first)
  {
    return 1;
  }
  auto backup = CloudBackup(config.second);
  return 0;
}

