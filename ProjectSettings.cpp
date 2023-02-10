#include <filesystem>
#include <iostream>

#include "ProjectSettings.h"

namespace fs = std::filesystem;

const string
PROJECT::PHOTO_LIBRARY_ALT ()
{
  auto path = string (getenv ("HOME")) + "/Pictures";
  for (const auto &entry : fs::directory_iterator (path))
    {
      auto filename = entry.path ().filename ().string ();
      if (filename.find (".photoslibrary") != string::npos)
        {
          std::cout << path + "/" + filename << std::endl;
          return path + "/" + filename;
        }
    }
  return PHOTO_LIBRARY;
}
