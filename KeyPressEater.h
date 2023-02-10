#ifndef KEYPRESSEATER_H
#define KEYPRESSEATER_H

#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QTableView>

class KeyPressEater : public QObject
{
  Q_OBJECT
private:
  QTableView *_target_view{};

protected:
  bool eventFilter (QObject *obj, QEvent *event) override;

public:
  KeyPressEater (QTableView *view, QObject *parent);
};

#endif // KEYPRESSEATER_H
