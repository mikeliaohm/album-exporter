#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include <string>

using std::string;

namespace PROJECT
{
  /* Looks up the full directory name of the photo library. */
  const string PHOTO_LIBRARY_ALT ();

  const string PHOTO_LIBRARY = string (getenv ("HOME"))
      + "/Pictures/Photos Library.photoslibrary";

  const string PHOTO_ORIGINALS = PHOTO_LIBRARY_ALT () + "/originals";

  const string PHOTO_DB_PATH = PHOTO_LIBRARY_ALT () + "/database";

  const string PHOTO_DB_NAME = "Photos.sqlite";

  const string PREVIEW_APP = "/usr/bin/qlmanage";
}

#endif // PROJECTSETTINGS_H
