#include "Storage.hpp"

#include "FileVec/util/PathUtil.hpp"

#include <cstdio>
#include <string>

#ifdef _WIN32
#include <windows.h>

File::Storage::data File::Storage::checkStorage()
{
  const std::filesystem::path rootDirectory = Util::getTempDir().root_directory();
  const std::filesystem::path driveDirectory = Util::getTempDir().root_name();

  data storage;
  const std::string driveName = driveDirectory.string();

  if(GetDriveType(driveName.data()) != DRIVE_FIXED)
  {
    std::printf("not a fixed drive, skipping");
  }
  else
  {
    GetDiskFreeSpaceEx(driveName.data(), NULL, (PULARGE_INTEGER)&storage.total, (PULARGE_INTEGER)&storage.free);
  }

  return storage;
}
#elif __APPLE__
File::Storage::data File::Storage::checkStorage()
{
  return {};
}
#else
File::Storage::data File::Storage::checkStorage()
{
  return {};
}
#endif
