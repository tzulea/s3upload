#include <cstdlib>
#include <map>
#include <string>
#include <future>
#include "constants.h"
#include "CloudWatchBackup.h"
#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <signal.h>
#endif


using namespace s3upload;
std::promise<void> promiseWait;

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

#ifdef _WIN32
BOOL WINAPI handleSignal(
  _In_ DWORD dwCtrlType
)
{
  switch (dwCtrlType)
  {
    // Handle the CTRL-C signal.
  case CTRL_C_EVENT:
  case CTRL_CLOSE_EVENT:
  case CTRL_BREAK_EVENT:
  case CTRL_LOGOFF_EVENT:
  case CTRL_SHUTDOWN_EVENT:
  {
    promiseWait.set_value();
    break;
  }
  default:
    return FALSE;
  }
}
#elif __linux__
void handleSignal(int s) {
  promiseWait.set_value();
}
#endif


int main(int argc, char *argv[])
{
  // get config options from enviroment variable
  auto config = getConfig();

  if(!config.first)
  {
    return 1;
  }
  if (argc == 3)
  {

    // single file upload mode
    std::string pathArg = argv[1];
    std::string path = argv[2];
    if (pathArg == "-p")
    {
      CloudWatchBackup backup(config.second);
      backup.upload(path);
    }
    else if (pathArg == "-w")
    {
#ifdef _WIN32
      SetConsoleCtrlHandler(handleSignal, TRUE);
#elif __linux__
      struct sigaction sigIntHandler;

      sigIntHandler.sa_handler = handleSignal;
      sigemptyset(&sigIntHandler.sa_mask);
      sigIntHandler.sa_flags = 0;
#endif
      // watcher mode
      CloudWatchBackup backup(config.second);
      backup.watch(path);

      std::future<void> future = promiseWait.get_future();
      future.get(); // wait until user closes the app;
    }
    else
    {
      std::cerr << "wrong arguments, please use the form s3Upload -p apath or"<<std::endl;
      std::cerr << "s3Upload -w apath" << std::endl;
    }
  }
  else
  {
    std::cerr << "wrong arguments, please use the form s3Upload -p apath or" << std::endl;
    std::cerr << "s3Upload -w apath" << std::endl;
  }
  return 0;
}
