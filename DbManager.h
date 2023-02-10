#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <vector>
#include <list>

#define DB_ERR -1

struct PhotoTablePK
{
  int album_table_pk{ DB_ERR };
  int asset_table_pk{ DB_ERR };
};

struct PhotoFile
{
  std::string dir;
  std::string filename;
};

struct Album
{
  std::string title;
  std::vector<struct PhotoFile> photos;
};

class DbManager
{
private:
  QSqlDatabase _db;                    /* Instance of the database. */
  struct PhotoTablePK _photo_table_PK; /* Tracks the revelant table PKs */
  const QString partial_photo_query ();
public:
  /* Constructs the manager and stores table PKs. */
  DbManager (const QString &db_path);

  DbManager ();

  /* Fetches albums and their containing photos. */
  const QString album_list_query ();

  /* Fetches photos based on ALBUM_PK. */
  const QString photo_query (const int album_PK);

  /* Fetches photos across multiple albums based on ALBUM_PK_LIST. */
  const QString bulk_photo_query (const std::list<int> &album_PK_list);

  /* Returns the SQL statement strings. */
  const QString z_asset_table ();
  const QString z_asset_pk ();
  const QString z_asset_directory ();
  const QString z_asset_filename ();
  const QString album_asset_table ();
  const QString album_table ();
  const QString asset_table ();
  const QSqlDatabase &db ();
};

#endif // DBMANAGER_H
