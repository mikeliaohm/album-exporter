#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

/* Headers used to access protected folder to trigger
   the permission requests in MacOS. */
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "ProjectSettings.h"

#define _GNU_SOURCE

int main(int argc, char *argv[])
{
  /* Request for Photos permission. Snippet from
     https://www.qt.io/blog/the-curious-case-of-the-responsible-process.
     Adjusted to use stringstream instead. */
  if (DIR *photo_dir = opendir (PROJECT::PHOTO_LIBRARY_ALT ().c_str ()))
    {
      qDebug ("Photos access granted\n");
      closedir (photo_dir);
    }
  else
    {
      qDebug ("Photos access denied\n");
    }
//  free (photoPath);

  QApplication a(argc, argv);

  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  for (const QString &locale : uiLanguages) {
      const QString baseName = "album-exporter_" + QLocale(locale).name();
      if (translator.load(":/i18n/" + baseName)) {
          a.installTranslator(&translator);
          break;
        }
    }

  MainWindow w;
  w.show();
  return a.exec();
}
