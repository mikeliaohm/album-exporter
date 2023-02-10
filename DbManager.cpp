#include "DbManager.h"
#include <QDebug>
#include <QFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QString>

#include "ProjectSettings.h"

/* Fetches the Asset table PK and Album table PK from DB */
static struct PhotoTablePK fetch_photo_table_PK (const QSqlDatabase &db);

DbManager::DbManager (const QString &path)
{
  auto download_path = QString::fromStdString (PROJECT::PHOTO_DB_PATH);
  qDebug () << "the folder location\t" << download_path;
  auto db_full_path = download_path + "/" + path;

  QFile file (db_full_path);
  if (!file.exists ())
    {
      qDebug () << "File does not exist!";
      return;
    }

  _db = QSqlDatabase::addDatabase ("QSQLITE");
  _db.setDatabaseName (db_full_path);

  if (!_db.open ())
    {
      qDebug () << "Error: connection with DB failed";
      return;
    }
  else
    {
      qDebug () << "Db connected!";
    }

  _photo_table_PK = fetch_photo_table_PK (_db);

  qDebug () << "The asset PK is: " << _photo_table_PK.asset_table_pk;
  qDebug () << "The album PK is: " << _photo_table_PK.album_table_pk;
}

DbManager::DbManager () {}

static struct PhotoTablePK
fetch_photo_table_PK (const QSqlDatabase &db)
{
  /* Queries pk for the Album table and Asset table. */
  QSqlQuery query{ db };

  /* Need to call lastError after setForward is called. */
  query.setForwardOnly (true);
  query.exec ("SELECT Z_ENT, Z_NAME from Z_PRIMARYKEY where Z_NAME = "
              "'album' COLLATE NOCASE OR Z_NAME = 'asset' COLLATE NOCASE");

  struct PhotoTablePK photo_table_PK;

  /* Check if two rows are returned. Retrieve record before
     fetch the row count. */
  if (query.record ().count () < 1)
    {
      qDebug ()
          << "Should've returned two rows, one for Album and one for Asset";
      return photo_table_PK;
    }

  while (query.next ())
    {
      if (query.value (1).toString () == "Asset")
        photo_table_PK.asset_table_pk = query.value (0).toInt ();
      else
        photo_table_PK.album_table_pk = query.value (0).toInt ();
    }

  return photo_table_PK;
}

const QSqlDatabase &
DbManager::db ()
{
  return _db;
}

const QString
DbManager::album_list_query ()
{
  return "SELECT Z_PK, Z_ENT, ZTITLE FROM ZGENERICALBUM "
         "WHERE ZTITLE IS NOT NULL "
         "ORDER BY ZTITLE";
}

const QString
DbManager::partial_photo_query ()
{
  QString query_str { "SELECT " };
  query_str.append (album_table ()).append (", ");
  query_str.append (asset_table ()).append (", ");
  query_str.append (z_asset_pk ()).append (", ");
  query_str.append (z_asset_directory ()).append (", ");
  query_str.append (z_asset_filename ());
  query_str.append (" FROM ").append (album_asset_table ());
  query_str.append (" INNER JOIN ");
  query_str.append (z_asset_table ());
  query_str.append (" ON ").append (asset_table ()).append (" = ")
      .append (z_asset_pk ());
  query_str.append (" AND ").append (album_table());

  return query_str;
}

const QString
DbManager::photo_query (const int album_PK)
{
  QString query_str = partial_photo_query ();
  query_str.append (" = ").append (QString::number (album_PK));
  return query_str;
}

const QString
DbManager::bulk_photo_query (const std::list<int> &album_PK_list)
{
  QString query_str = partial_photo_query ();
  query_str.append (" IN (");

  for (auto it = album_PK_list.cbegin (); it != album_PK_list.cend (); ++it)
    {
      query_str.append (QString::number (*it));
      if (it != --album_PK_list.cend ())
        query_str.append (", ");
    }
  query_str.append (")");

  return query_str;
}

const QString
DbManager::z_asset_table ()
{
  return "ZASSET";
}

const QString
DbManager::z_asset_pk ()
{
  return QString{ z_asset_table () + ".Z_PK" };
}

const QString
DbManager::z_asset_directory ()
{
  return QString{ z_asset_table () + ".ZDIRECTORY" };
}

const QString
DbManager::z_asset_filename ()
{
  return QString{ z_asset_table () + ".ZFILENAME" };
}

const QString
DbManager::album_asset_table ()
{
  QString album_asset_table{ "Z_" };
  album_asset_table.append (QString::number (_photo_table_PK.album_table_pk));
  album_asset_table.append ("ASSETS");
  return album_asset_table;
}

const QString
DbManager::album_table ()
{
  QString album_table{ album_asset_table () + ".Z_" };
  album_table.append (QString::number (_photo_table_PK.album_table_pk));
  album_table.append ("ALBUMS");
  return album_table;
}

const QString
DbManager::asset_table ()
{
  QString asset_table{ album_asset_table () + ".Z_" };
  asset_table.append (QString::number (_photo_table_PK.asset_table_pk));
  asset_table.append ("ASSETS");
  return asset_table;
}
