#ifndef CHECKABLESQLQUERYMODEL_H
#define CHECKABLESQLQUERYMODEL_H

#include <QSqlQueryModel>

/* Inhirits QSqlQueryModel and overwrite the flags, data, and
   setData member functions to support checkable state in the
   model items. */
class CheckableSqlQueryModel : public QSqlQueryModel
{
public:
    CheckableSqlQueryModel (QObject* parent = 0);
    Qt::ItemFlags flags (const QModelIndex& index) const;
    QVariant data (const QModelIndex &index, int role) const;
    bool setData (const QModelIndex &index, const QVariant &value,
                  int role);

    const QSet<QPersistentModelIndex> &selected_items ();
private:
    QSet<QPersistentModelIndex> _checked_items;
};

#endif // CHECKABLESQLQUERYMODEL_H
