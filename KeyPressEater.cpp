#include <QProcess>
#include <QStandardPaths>
#include <QTableView>

#include "KeyPressEater.h"
#include "ProjectSettings.h"

/* Fetch the media path the selected item in OBJ (a QTableView instance). */
static void preview_media (QObject *obj);

/* Spawn a subprocess to preview media in MEDIA_PATH */
static void spawn_proc (const QString &media_path);

KeyPressEater::KeyPressEater (QTableView *view, QObject *parent)
    : QObject (parent)
{
  _target_view = view;
}

bool
KeyPressEater::eventFilter (QObject *obj, QEvent *event)
{
  if (event->type () == QEvent::KeyPress)
    {
      /* Cast obj to QTableView when the object sending the
         key event is matched. */
      if (obj != nullptr && obj == _target_view)
        {
          QKeyEvent *key_event = static_cast<QKeyEvent *> (event);
          switch (key_event->key ())
            {
            case Qt::Key_Space:
              preview_media (obj);
              break;
            default:
              break;
            }
          return true;
        }
    }

  return QObject::eventFilter (obj, event);
}

static void
preview_media (QObject *obj)
{
  QTableView *view = static_cast<QTableView *> (obj);
  auto selection_model = view->selectionModel ();

  /* No-op if there is no selection. */
  if (selection_model == nullptr || !selection_model->hasSelection ())
    return;

  auto current_idx = selection_model->currentIndex ();
  auto folder_idx = current_idx.siblingAtColumn (3);
  auto filename_idx = current_idx.siblingAtColumn (4);
  auto selected = view->model ();
  auto folder = selected->data (folder_idx).toString ();
  auto filename = selected->data (filename_idx).toString ();

  spawn_proc (folder + "/" + filename);
}

static void
spawn_proc (const QString &media_path)
{
  const QString program = QString::fromStdString (PROJECT::PREVIEW_APP);
  const QString path_prefix
      = QString::fromStdString (PROJECT::PHOTO_ORIGINALS);
  QStringList arguments;
  arguments << "-p" << path_prefix + "/" + media_path;

  QProcess proc{};
  proc.start (program, arguments);

  /* Main process will block until the previous process ends. */
  if (!proc.waitForFinished ())
    qDebug () << "something's wrong";
}
