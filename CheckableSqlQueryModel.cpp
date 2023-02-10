#include <QColor>

#include "CheckableSqlQueryModel.h"

CheckableSqlQueryModel::CheckableSqlQueryModel (QObject* parent)
  : QSqlQueryModel (parent) { }

Qt::ItemFlags
CheckableSqlQueryModel::flags (const QModelIndex & index) const
{
  Qt::ItemFlags defaultFlags = QSqlQueryModel::flags (index);
    if (index.isValid()){
      return defaultFlags | Qt::ItemIsUserCheckable;
    }
  return defaultFlags;
}

QVariant
CheckableSqlQueryModel::data (const QModelIndex &index, int role) const
{
  if (index.isValid () && role == Qt::CheckStateRole)
    return _checked_items.contains (index) ? Qt::Checked : Qt::Unchecked;

  return QSqlQueryModel::data (index, role);
}

bool
CheckableSqlQueryModel::setData (const QModelIndex &index,
                                 const QVariant &value, int role)
{
  if (!index.isValid () || role != Qt::CheckStateRole)
    return false;

  if (value == Qt::Checked)
    _checked_items.insert (index);
  else
    _checked_items.remove (index);

  emit dataChanged (index, index);

  return true;
}

const QSet<QPersistentModelIndex> &
CheckableSqlQueryModel::selected_items ()
{
  return _checked_items;
}
